#ifndef IR_REMOTE_H
#define IR_REMOTE_H

// 依赖底层运动模块
#include "../basic_move/basic_move.h"
#include <wiringPi.h>
#include <softPwm.h>
#include <lirc/lirc_client.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 遥控器全部按键映射表（和lircd.conf一一对应）
#define KEY_MAP_COUNT 21
extern char *keymap[KEY_MAP_COUNT];

// 按键动作解析函数
void ircontrol(char *code);

// 红外遥控整套硬件+lirc服务初始化
int ir_remote_all_init(void);

// 阻塞式红外遥控主循环，持续监听遥控器按键
void ir_remote_listen_loop(void);

#endif