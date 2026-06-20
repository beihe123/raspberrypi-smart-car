#ifndef PROTOCOL_DEF_H
#define PROTOCOL_DEF_H

// PC端3字节指令头标识
#define PC_HEAD1 'O'
#define PC_HEAD2 'N'
// PC 3字节指令动作码
#define PC_FORWARD  'A'
#define PC_BACK     'B'
#define PC_LEFT     'C'
#define PC_RIGHT    'D'
#define PC_STOP     'E'
// PC云台扩展指令
#define PTZ_LEFT    'L'
#define PTZ_RIGHT   'I'
#define PTZ_UP      'K'
#define PTZ_DOWN    'J'

// 手机端6字节协议偏移
#define MOBILE_HEAD_OFFSET 2
#define MOBILE_FORWARD  0x01
#define MOBILE_BACK     0x02
#define MOBILE_LEFT     0x03
#define MOBILE_RIGHT    0x04
#define MOBILE_STOP     0x00

#define BUFSIZE 512
#define MAX_CLIENTS FD_SETSIZE
// 云台舵机通道
#define PTZ_LR_CH 1
#define PTZ_UD_CH 2
// 云台角度步长
#define PTZ_STEP 10
#define PTZ_MIN_ANG 0
#define PTZ_MAX_ANG 180

#endif