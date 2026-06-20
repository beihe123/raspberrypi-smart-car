/*
 * 任务7：智能小车驱动接口
 * 知识点：树莓派GPIO、wiringPi库、电机底层驱动封装
 * 采用wiringPi编号初始化，适配直流减速电机L298N/L293D驱动板
*/
#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>

// ===================== 小车电机wiringPi引脚定义 =====================
// 左电机控制引脚
#define LEFT_MOTOR_IN1  0
#define LEFT_MOTOR_IN2  1
#define LEFT_MOTOR_PWM  2

// 右电机控制引脚
#define RIGHT_MOTOR_IN1 3
#define RIGHT_MOTOR_IN2 4
#define RIGHT_MOTOR_PWM 5

// PWM参数定义（软件PWM范围0~100）
#define PWM_RANGE  100
#define PWM_STOP   0
#define PWM_SLOW   30
#define PWM_MID    60
#define PWM_FAST   90

// ===================== 函数声明 =====================
// 底层硬件初始化（任务7核心：wiringPi初始化+GPIO模式配置）
int MotorHardwareInit(void);

// 电机基础动作控制
void MotorStop(void);
void MotorForward(int speed);
void MotorBackward(int speed);
void MotorTurnLeft(int speed);
void MotorTurnRight(int speed);
void MotorSlightLeft(int speed);
void MotorSlightRight(int speed);

// ===================== 硬件初始化实现 =====================
int MotorHardwareInit(void)
{
    // wiringPi初始化，使用wiringPi编号模式（任务7重点函数 wiringPiSetup）
    int ret = wiringPiSetup();
    if (ret == -1)
    {
        printf("wiringPi初始化失败！请检查权限，使用sudo运行程序\n");
        return -1;
    }
    printf("wiringPi初始化成功\n");

    // 设置方向引脚为普通输出
    pinMode(LEFT_MOTOR_IN1, OUTPUT);
    pinMode(LEFT_MOTOR_IN2, OUTPUT);
    pinMode(RIGHT_MOTOR_IN1, OUTPUT);
    pinMode(RIGHT_MOTOR_IN2, OUTPUT);

    // 软件PWM引脚初始化（突破硬件PWM引脚限制）
    softPwmCreate(LEFT_MOTOR_PWM, PWM_STOP, PWM_RANGE);
    softPwmCreate(RIGHT_MOTOR_PWM, PWM_STOP, PWM_RANGE);

    // 上电默认电机停止
    MotorStop();
    printf("小车电机GPIO驱动接口初始化完成\n");
    return 0;
}

// ===================== 电机动作函数实现 =====================
// 全部电机停止
void MotorStop(void)
{
    digitalWrite(LEFT_MOTOR_IN1, LOW);
    digitalWrite(LEFT_MOTOR_IN2, LOW);
    digitalWrite(RIGHT_MOTOR_IN1, LOW);
    digitalWrite(RIGHT_MOTOR_IN2, LOW);
    softPwmWrite(LEFT_MOTOR_PWM, PWM_STOP);
    softPwmWrite(RIGHT_MOTOR_PWM, PWM_STOP);
}

// 整车前进 speed：0~100
void MotorForward(int speed)
{
    digitalWrite(LEFT_MOTOR_IN1, HIGH);
    digitalWrite(LEFT_MOTOR_IN2, LOW);
    digitalWrite(RIGHT_MOTOR_IN1, HIGH);
    digitalWrite(RIGHT_MOTOR_IN2, LOW);
    softPwmWrite(LEFT_MOTOR_PWM, speed);
    softPwmWrite(RIGHT_MOTOR_PWM, speed);
}

// 整车后退
void MotorBackward(int speed)
{
    digitalWrite(LEFT_MOTOR_IN1, LOW);
    digitalWrite(LEFT_MOTOR_IN2, HIGH);
    digitalWrite(RIGHT_MOTOR_IN1, LOW);
    digitalWrite(RIGHT_MOTOR_IN2, HIGH);
    softPwmWrite(LEFT_MOTOR_PWM, speed);
    softPwmWrite(RIGHT_MOTOR_PWM, speed);
}

// 原地左转
void MotorTurnLeft(int speed)
{
    digitalWrite(LEFT_MOTOR_IN1, LOW);
    digitalWrite(LEFT_MOTOR_IN2, HIGH);
    digitalWrite(RIGHT_MOTOR_IN1, HIGH);
    digitalWrite(RIGHT_MOTOR_IN2, LOW);
    softPwmWrite(LEFT_MOTOR_PWM, speed);
    softPwmWrite(RIGHT_MOTOR_PWM, speed);
}

// 原地右转
void MotorTurnRight(int speed)
{
    digitalWrite(LEFT_MOTOR_IN1, HIGH);
    digitalWrite(LEFT_MOTOR_IN2, LOW);
    digitalWrite(RIGHT_MOTOR_IN1, LOW);
    digitalWrite(RIGHT_MOTOR_IN2, HIGH);
    softPwmWrite(LEFT_MOTOR_PWM, speed);
    softPwmWrite(RIGHT_MOTOR_PWM, speed);
}

// 前进小幅左拐（差速转向）
void MotorSlightLeft(int speed)
{
    digitalWrite(LEFT_MOTOR_IN1, HIGH);
    digitalWrite(LEFT_MOTOR_IN2, LOW);
    digitalWrite(RIGHT_MOTOR_IN1, HIGH);
    digitalWrite(RIGHT_MOTOR_IN2, LOW);
    softPwmWrite(LEFT_MOTOR_PWM, speed * 0.4);
    softPwmWrite(RIGHT_MOTOR_PWM, speed);
}

// 前进小幅右拐（差速转向）
void MotorSlightRight(int speed)
{
    digitalWrite(LEFT_MOTOR_IN1, HIGH);
    digitalWrite(LEFT_MOTOR_IN2, LOW);
    digitalWrite(RIGHT_MOTOR_IN1, HIGH);
    digitalWrite(RIGHT_MOTOR_IN2, LOW);
    softPwmWrite(LEFT_MOTOR_PWM, speed);
    softPwmWrite(RIGHT_MOTOR_PWM, speed * 0.4);
}

// ===================== 测试主函数（可单独编译运行） =====================
int main(void)
{
    if(MotorHardwareInit() != 0)
        return 1;

    printf("前进3秒\n");
    MotorForward(PWM_MID);
    delay(3000);

    printf("原地左转2秒\n");
    MotorTurnLeft(PWM_SLOW);
    delay(2000);

    printf("原地右转2秒\n");
    MotorTurnRight(PWM_SLOW);
    delay(2000);

    printf("后退3秒\n");
    MotorBackward(PWM_MID);
    delay(3000);

    printf("电机停止\n");
    MotorStop();
    return 0;
}