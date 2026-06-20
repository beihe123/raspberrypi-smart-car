#ifndef IR_OBSTACLE_H
#define IR_OBSTACLE_H

// 依赖底层硬件驱动与基础运动模块
#include "../hardware_driver/status_led.c"
#include "../hardware_driver/alarm_buzzer.c"
#include "../hardware_driver/key_input.c"
#include "../basic_move/basic_move.h"
#include <wiringPi.h>
#include <softPwm.h>

// 任务15硬件引脚定义（文档BCM转wiringPi）
// 左避障探头 BCM12 → wiringPi 26
// 右避障探头 BCM16 → wiringPi 27
#define IR_OBSTACLE_LEFT    26
#define IR_OBSTACLE_RIGHT   27

// 避障状态枚举
typedef enum{
    OBSTACLE_GO,        // 无障碍物，直行
    OBSTACLE_TURN_L,    // 右侧障碍，向左转
    OBSTACLE_TURN_R,    // 左侧障碍，向右转
    OBSTACLE_BACK_TURN  // 双侧均有障碍，后退再转向
}ObstacleState;

// 全套硬件初始化：避障传感器+电机+指示灯+蜂鸣+按键
int ir_all_hard_init(void);

// 读取左右两路红外避障传感器电平
void ir_read_sensor(int *sl, int *sr);

// 任务16核心：全自动红外避障主循环
void ir_auto_obstacle_run(unsigned int speed);

#endif