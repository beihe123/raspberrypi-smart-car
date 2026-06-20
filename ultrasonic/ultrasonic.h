#ifndef ULTRASONIC_H
#define ULTRASONIC_H

// 依赖底层模块
#include "../hardware_driver/status_led.c"
#include "../hardware_driver/alarm_buzzer.c"
#include "../hardware_driver/key_input.c"
#include "../basic_move/basic_move.h"
#include "pca9685.h"
#include <wiringPi.h>
#include <softPwm.h>
#include <sys/time.h>

// ========== 任务17/18 HC-SR04超声波引脚 wiringPi编号 ==========
#define ULTRA_TRIG 28
#define ULTRA_ECHO 29

// ========== 任务20 PCA9685舵机配置常量 ==========
#define PIN_BASE    300
#define PCA_ADDR    0x40
#define SERVO_FREQ  50
#define MAX_PWM_TICK 4096
#define ULTRA_SERVO_CH 0  // 超声波摇头舵机通道0

// 工具宏
#define max(x,y) ((x)>(y)?(x):(y))
#define min(x,y) ((x)<(y)?(x):(y))

// 硬件总初始化：超声波+PCA9685舵机+整车电机/LED/蜂鸣/按键
int ultra_servo_all_init(void);

// 任务18 超声波测距核心
void ultra_sensor_init(void);
float ultra_measure_dis(void);

// 任务20 舵机控制接口
int calcTicks(float impulseMs, int hertz);
void servo_set_angle(int ch, float angle);
void servo_scan_left(void);
void servo_scan_front(void);
void servo_scan_right(void);

// 任务19 固定前方超声波避障（无舵机）
void ultra_simple_obstacle(unsigned int speed, float danger_dis);

// 任务20 舵机多方位扫描智能避障
void servo_scan_obstacle_run(unsigned int speed, float danger_dis);

#endif