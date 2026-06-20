#ifndef TRACKING_LINE_H
#define TRACKING_LINE_H

// 依赖底层驱动与基础运动模块
#include "../hardware_driver/status_led.c"
#include "../hardware_driver/alarm_buzzer.c"
#include "../hardware_driver/key_input.c"
#include "../basic_move/basic_move.h"
#include <wiringPi.h>
#include <softPwm.h>

// 任务13硬件引脚定义（文档规定BCM→wiringPi）
// 左循迹探头 BCM13 → wiringPi 23
// 右循迹探头 BCM26 → wiringPi 25
#define TRACK_LEFT     23
#define TRACK_RIGHT    25

// 循迹状态枚举
typedef enum{
    TRACK_STOP,     // 无黑线，停车
    TRACK_GO,       // 双探头白线，直行
    TRACK_TURN_L,   // 左探头黑线，向右修正
    TRACK_TURN_R    // 右探头黑线，向左修正
}TrackState;

// 硬件初始化：循迹传感器+整车全部硬件
int track_all_hard_init(void);

// 读取两路红外探头状态
void track_read_sensor(int *sl, int *sr);

// 任务14核心：自动循迹主循环
void track_auto_run(unsigned int speed);

#endif