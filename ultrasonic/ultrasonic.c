/*
模块：ultrasonic 超声波+PCA9685舵机避障
任务17：HC-SR04超声波硬件原理、电路分析、测距理论
任务18：超声波GPIO初始化、单次测距代码实现
任务19：固定前方超声波自动避障行驶
任务20：PCA9685 I2C舵机驱动、左右摇头扫描、多方位智能避障
硬件说明：
1. HC-SR04：Trig=28(OUT) Echo=29(IN)，测距公式d=34990*t/2 cm
2. PCA9685 I2C地址0x40，50Hz PWM，通道0挂载超声波舵机
3. 舵机角度：5°右前 / 90°正前 / 175°左前
*/
#include "ultrasonic.h"
#include <stdio.h>

// 1. 任务17/18 超声波传感器初始化
void ultra_sensor_init(void)
{
    pinMode(ULTRA_ECHO, INPUT);
    pinMode(ULTRA_TRIG, OUTPUT);
    digitalWrite(ULTRA_TRIG, LOW);
    printf("HC-SR04超声波传感器初始化完成\n");
}

// 任务18 测距函数，返回厘米距离
float ultra_measure_dis(void)
{
    struct timeval tv1, tv2;
    long time_start, time_stop;
    float distance;
    // 发送10us触发脉冲
    digitalWrite(ULTRA_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(ULTRA_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRA_TRIG, LOW);
    // 捕获Echo电平时间差
    while(!digitalRead(ULTRA_ECHO));
    gettimeofday(&tv1, NULL);
    while(digitalRead(ULTRA_ECHO));
    gettimeofday(&tv2, NULL);
    time_start = tv1.tv_sec * 1000000 + tv1.tv_usec;
    time_stop  = tv2.tv_sec * 1000000 + tv2.tv_usec;
    distance = (float)(time_stop - time_start) / 1000000 * 34990 / 2;
    return distance;
}

// 2. 任务20 舵机换算工具函数
int calcTicks(float impulseMs, int hertz)
{
    float cycle = 1000.0f / hertz;
    return (int)(MAX_PWM_TICK * impulseMs / cycle + 0.5f);
}

// 设置指定通道舵机角度 0~180°
void servo_set_angle(int ch, float angle)
{
    float pulse_ms = angle / 90.0f + 0.5f;
    pulse_ms = max(pulse_ms, 0.5f);
    pulse_ms = min(pulse_ms, 2.5f);
    int tick = calcTicks(pulse_ms, SERVO_FREQ);
    pwmWrite(PIN_BASE + ch, tick);
    delay(300); // 等待舵机稳定
}

// 舵机扫描三个方向
void servo_scan_left(void)  { servo_set_angle(ULTRA_SERVO_CH, 175); }
void servo_scan_front(void) { servo_set_angle(ULTRA_SERVO_CH, 90);  }
void servo_scan_right(void) { servo_set_angle(ULTRA_SERVO_CH, 5);   }

// 整车硬件统一初始化：超声波+PCA9685舵机+电机+LED+蜂鸣+按键
int ultra_servo_all_init(void)
{
    if(wiringPiSetup() == -1)
    {
        printf("wiringPi初始化失败，请sudo运行\n");
        return -1;
    }
    ultra_sensor_init();
    // PCA9685初始化
    int pca_fd = pca9685Setup(PIN_BASE, PCA_ADDR, SERVO_FREQ);
    if(pca_fd < 0)
    {
        printf("PCA9685 I2C设备初始化失败！检查接线\n");
        return -1;
    }
    pca9685PWMReset(pca_fd);
    // 复用基础运动模块硬件初始化
    car_all_hardware_init();
    servo_scan_front(); // 上电舵机回正前方
    printf("超声波+舵机整车硬件初始化完成\n");
    return 0;
}

// 3. 任务19 基础单方向超声波避障（无舵机，只测正前方）
void ultra_simple_obstacle(unsigned int speed, float danger_dis)
{
    float dis;
    while(1)
    {
        dis = ultra_measure_dis();
        printf("前方距离：%.2f cm\n", dis);
        if(dis < danger_dis)
        {
            printf("前方遇障，后退避让\n");
            buzzer_short_warn();
            t_down(speed, 300);
            t_left(speed, 300);
        }
        else
        {
            t_up(speed, 10);
        }
        delay(200);
    }
}

// 4. 任务20 舵机多方位扫描智能避障（左右摇头测距判断最优避让方向）
void servo_scan_obstacle_run(unsigned int speed, float danger_dis)
{
    float dis_front, dis_left, dis_right;
    while(1)
    {
        // 分三次扫描左/前/右
        servo_scan_front();
        dis_front = ultra_measure_dis();
        servo_scan_left();
        dis_left = ultra_measure_dis();
        servo_scan_right();
        dis_right = ultra_measure_dis();
        servo_scan_front();
        printf("前:%.1f 左:%.1f 右:%.1f cm\n", dis_front, dis_left, dis_right);

        if(dis_front >= danger_dis)
        {
            // 前方无障碍，直行
            t_up(speed, 10);
        }
        else
        {
            // 前方遇障，停车后退，对比左右距离选择转向
            t_stop(200);
            t_down(speed, 500);
            t_stop(200);
            if(dis_left > dis_right)
                t_left(speed, 400);
            else
                t_right(speed, 400);
        }
        delay(300);
    }
}

// 程序测试入口：按键切换两种避障模式
int main(void)
{
    if(ultra_servo_all_init() != 0)
        return 1;
    printf("超声波舵机避障程序启动\n");
    printf("按键1次：简单前方超声波避障；再按：舵机多方位扫描避障；再按停止\n");
    int run_mode = 0;
    while(1)
    {
        if(digitalRead(KEY_PIN) == 1)
        {
            delay(10);
            if(digitalRead(KEY_PIN) == 1)
            {
                while(digitalRead(KEY_PIN) == 1) delay(10);
                run_mode = (run_mode + 1) % 3;
                t_stop(0);
                switch(run_mode)
                {
                    case 0:
                        printf("程序停止\n");
                        break;
                    case 1:
                        printf("启动【任务19】基础超声波避障\n");
                        ultra_simple_obstacle(50, 40);
                        break;
                    case 2:
                        printf("启动【任务20】舵机多方位扫描避障\n");
                        servo_scan_obstacle_run(50, 40);
                        break;
                }
            }
        }
    }
    return 0;
}