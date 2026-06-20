/*
模块：basic_move 基础运动控制
任务11：实现小车前进、后退、停止；TB6612电机驱动、直流电机原理、直行逻辑
任务12：实现小车原地左拐、右拐；差速转向逻辑
硬件：双TB6612FNG驱动四轮直流电机
联动：行驶状态自动切换红绿LED、倒车蜂鸣报警、按键一键启停整套行驶流程
*/
#include "basic_move.h"
#include <stdio.h>

// TB6612电机硬件单独初始化
static int tb6612_motor_init(void)
{
    if(wiringPiSetup() == -1)
    {
        printf("wiringPi初始化失败，请使用sudo运行程序！\n");
        return -1;
    }
    // 全部方向引脚设为输出
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    // 软件PWM初始化，范围0~100
    softPwmCreate(PWMA, 0, PWM_MAX);
    softPwmCreate(PWMB, 0, PWM_MAX);
    // 上电默认全车停止
    t_stop(100);
    printf("TB6612四轮电机驱动初始化完成\n");
    return 0;
}

// 整车全部硬件统一初始化（电机+指示灯+蜂鸣+按键）
int car_all_hardware_init(void)
{
    if(tb6612_motor_init() != 0)
        return -1;
    led_init();
    buzzer_init();
    key_hardware_init();
    printf("整车所有底层硬件初始化完毕\n");
    return 0;
}

//==================== 任务11 前进、后退、停止 ====================
/**
 * @brief 小车全速直行前进
 * @param speed 行驶速度 0~100
 * @param t_time 持续行驶时间，单位ms
 */
void t_up(unsigned int speed, unsigned int t_time)
{
    // TB6612直行逻辑：AIN1=1 AIN2=0 / BIN1=1 BIN2=0
    digitalWrite(AIN2, LOW);
    digitalWrite(AIN1, HIGH);
    softPwmWrite(PWMA, speed);

    digitalWrite(BIN2, LOW);
    digitalWrite(BIN1, HIGH);
    softPwmWrite(PWMB, speed);

    green_led_on();
    red_led_off();
    delay(t_time);
}

/**
 * @brief 小车全速直行后退
 * @param speed 行驶速度 0~100
 * @param t_time 持续行驶时间，单位ms
 */
void t_down(unsigned int speed, unsigned int t_time)
{
    // TB6612后退逻辑：AIN1=0 AIN2=1 / BIN1=0 BIN2=1
    digitalWrite(AIN2, HIGH);
    digitalWrite(AIN1, LOW);
    softPwmWrite(PWMA, speed);

    digitalWrite(BIN2, HIGH);
    digitalWrite(BIN1, LOW);
    softPwmWrite(PWMB, speed);

    red_led_on();
    green_led_off();
    buzzer_short_warn(); // 倒车短促报警
    delay(t_time);
}

/**
 * @brief 小车全车电机停止
 * @param t_time 保持停止时长，单位ms
 */
void t_stop(unsigned int t_time)
{
    // TB6612停止逻辑：全部IN引脚置低，PWM输出0
    digitalWrite(AIN2, LOW);
    digitalWrite(AIN1, LOW);
    softPwmWrite(PWMA, 0);

    digitalWrite(BIN2, LOW);
    digitalWrite(BIN1, LOW);
    softPwmWrite(PWMB, 0);

    led_all_off();
    softToneWrite(BUZ_PIN, 0);
    delay(t_time);
}

//==================== 任务12 原地左转、原地右转 ====================
/**
 * @brief 小车原地左转（左轮反转，右轮正转）
 * @param speed 转向速度 0~100
 * @param t_time 转向持续时间，单位ms
 */
void t_left(unsigned int speed, unsigned int t_time)
{
    digitalWrite(AIN2, HIGH);
    digitalWrite(AIN1, LOW);
    softPwmWrite(PWMA, speed);

    digitalWrite(BIN2, LOW);
    digitalWrite(BIN1, HIGH);
    softPwmWrite(PWMB, speed);

    green_led_on();
    red_led_off();
    delay(t_time);
}

/**
 * @brief 小车原地右转（左轮正转，右轮反转）
 * @param speed 转向速度 0~100
 * @param t_time 转向持续时间，单位ms
 */
void t_right(unsigned int speed, unsigned int t_time)
{
    digitalWrite(AIN2, LOW);
    digitalWrite(AIN1, HIGH);
    softPwmWrite(PWMA, speed);

    digitalWrite(BIN2, HIGH);
    digitalWrite(BIN1, LOW);
    softPwmWrite(PWMB, speed);

    green_led_on();
    red_led_off();
    delay(t_time);
}

//==================== 拓展任务：按键一键触发整套行驶流程 ====================
void car_key_control_loop(void)
{
    int run_flag = 0;
    while(1)
    {
        // 按键消抖检测
        if(digitalRead(KEY_PIN) == 1)
        {
            delay(10);
            if(digitalRead(KEY_PIN) == 1)
            {
                run_flag = !run_flag;
                while(digitalRead(KEY_PIN) == 1) delay(10); // 等待松开
                if(run_flag == 1)
                {
                    printf("按键触发，小车开始完整行驶流程\n");
                    t_up(50, 2000);    // 前进2秒
                    t_down(50, 2000);  // 后退2秒
                    t_left(40, 2000);  // 左转2秒
                    t_right(40, 2000); // 右转2秒
                    t_stop(1000);      // 停止1秒
                    run_flag = 0;
                    printf("整套行驶动作完成，小车停止\n");
                }
            }
        }
    }
}

// 模块独立测试入口
int main(void)
{
    if(car_all_hardware_init() != 0)
        return 1;
    printf("基础运动控制程序启动，按下按键执行完整行驶动作\n");
    car_key_control_loop();
    return 0;
}