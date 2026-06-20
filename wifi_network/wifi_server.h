#ifndef WIFI_SERVER_H
#define WIFI_SERVER_H

#include "../basic_move/basic_move.h"
#include "../ultrasonic/pca9685.h"
#include "protocol_def.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <time.h>
#include <wiringPi.h>
#include <softPwm.h>

// 客户端连接结构体
typedef struct CLIENT {
    int fd;
    struct sockaddr_in addr;
} CLIENT;

// 全局云台角度
extern float g_lr_angle;
extern float g_ud_angle;

// 硬件全套初始化：电机+PCA9685云台+网络基础环境
int wifi_all_hard_init(void);

// 舵机角度控制封装
void ptz_set_lr(float angle);
void ptz_set_ud(float angle);

// 指令解析函数，区分PC3字节/手机6字节协议，同时处理小车+云台
void parse_command(char *buf, int len);

// TCP非阻塞多路服务主循环，入参监听端口
void wifi_tcp_server_loop(int port);

#endif