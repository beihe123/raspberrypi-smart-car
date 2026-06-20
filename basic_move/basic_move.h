#ifndef BASIC_MOVE_H
#define BASIC_MOVE_H

// 依赖底层硬件驱动
#include "../hardware_driver/motor_driver.c"
#include "../hardware_driver/status_led.c"
#include "../hardware_driver/alarm_buzzer.c"
#include "../hardware_driver/key_input.c"
#include <wiringPi.h>
#include <softPwm.h>

// TB6612FNG wiringPi引脚宏定义（文档规定）
#define PWMA    1
#define AIN2    2
#define AIN1    3
#define PWMB    4
#define BIN2    5
#define BIN1    6
#define PWM_MAX 100

// 整车运动状态枚举
typedef enum{
    CAR_STOP,
    CAR_FORWARD,    // 任务11 直行前进
    CAR_BACKWARD,   // 任务11 直行后退
    CAR_TURN_LEFT,  // 任务12 原地左转
    CAR_TURN_RIGHT  // 任务12 原地右转
}CarRunState;

// 硬件总初始化：TB6612电机+LED+蜂鸣器+按键
int car_all_hardware_init(void);

// 任务11 基础直行函数
void t_up(unsigned int speed, unsigned int t_time);    // 前进
void t_down(unsigned int speed, unsigned int t_time);  // 后退
void t_stop(unsigned int t_time);                      // 停止

// 任务12 原地转向函数
void t_left(unsigned int speed, unsigned int t_time);  // 原地左转
void t_right(unsigned int speed, unsigned int t_time); // 原地右转

// 按键控制整车自动行驶主循环（拓展任务：按键触发整套动作）
void car_key_control_loop(void);

#endif