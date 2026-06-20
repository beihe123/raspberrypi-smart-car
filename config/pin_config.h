#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

//==================== TB6612电机引脚 BOARD ====================
#define PWMA    12
#define AIN2    13
#define AIN1    15
#define PWMB    16
#define BIN2    18
#define BIN1    22

//==================== HC-SR04超声波引脚 ====================
#define ULTRA_TRIG 28
#define ULTRA_ECHO 29

//==================== PCA9685舵机全局配置 ====================
#define PIN_BASE    300
#define PCA_I2C_ADDR 0x40
#define SERVO_HZ    50
#define MAX_PWM_TICK 4096
// 舵机通道定义
#define ULTRA_SERVO_CH 0
#define PTZ_LR_CH      1
#define PTZ_UD_CH      2
// 舵机角度限制
#define SERVO_MIN_ANG 0
#define SERVO_MAX_ANG 180
#define SERVO_STEP    10

//==================== 网络、业务常量 ====================
#define WIFI_LISTEN_PORT 2001
#define BUFSIZE 512
#define OBSTACLE_DANGER_DIS 40.0f

#endif