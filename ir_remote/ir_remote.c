/*
模块：ir_remote 红外遥控器控制小车
任务21：红外接收模块硬件原理、LIRC解码库工作机制、遥控器按键编码分析
任务22：基于lirc_client库接收遥控按键码，5个按键控制小车前进/后退/左转/右转/停止
硬件说明：
1. 红外一体化接收头接树莓派GPIO，由系统lirc服务统一采集红外脉冲编码
2. LIRC库作用：解析红外脉冲，输出标准按键字符串，与配置文件lircd.conf匹配
3. 按键分配：
keymap[1]  KEY_CHANNEL    → 前进 t_up
keymap[7]  KEY_VOLUMEUP   → 后退 t_down
keymap[3]  KEY_PREVIOUS   → 左转 t_left
keymap[5]  KEY_PLAYPAUSE  → 右转 t_right
keymap[4]  KEY_NEXT       → 停止 t_stop
4. 按键消抖：两次按键间隔最小400ms，防止重复触发
*/
#include "ir_remote.h"

// 遥控器21组按键名映射，匹配lircd.conf配置
char *keymap[KEY_MAP_COUNT] ={
    "KEY_CHANNELDOWN",
    "KEY_CHANNEL",
    "KEY_CHANNELUP",
    "KEY_PREVIOUS",
    "KEY_NEXT",
    "KEY_PLAYPAUSE",
    "KEY_VOLUMEDOWN",
    "KEY_VOLUMEUP",
    "KEY_EQUAL",
    "KEY_NUMERIC_0",
    "BTN_0",
    "BTN_1",
    "KEY_NUMERIC_1",
    "KEY_NUMERIC_2",
    "KEY_NUMERIC_3",
    "KEY_NUMERIC_4",
    "KEY_NUMERIC_5",
    "KEY_NUMERIC_6",
    "KEY_NUMERIC_7",
    "KEY_NUMERIC_8",
    "KEY_NUMERIC_9"
};

/**
 * @brief 解析红外按键编码，执行对应小车动作
 * @param code lirc返回的按键字符串
 */
void ircontrol(char *code)
{
    if (strstr(code, keymap[1]))
    {
        printf("遥控按键：前进\n");
        t_up(50,10);
    }
    else if (strstr(code, keymap[7]))
    {
        printf("遥控按键：后退\n");
        t_down(50,10);
    }
    else if(strstr(code, keymap[3]))
    {
        printf("遥控按键：左转\n");
        t_left(50,10);
    }
    else if(strstr(code,keymap[5]))
    {
        printf("遥控按键：右转\n");
        t_right(50,10);
    }
    else if(strstr(code,keymap[4]))
    {
        printf("遥控按键：停止\n");
        t_stop(10);
    }
}

/**
 * @brief 整车硬件 + lirc客户端初始化
 * @return 0成功，-1失败
 */
int ir_remote_all_init(void)
{
    // 初始化wiringPi与电机、LED、蜂鸣硬件
    if(car_all_hardware_init() != 0)
    {
        printf("底层电机硬件初始化失败\n");
        return -1;
    }

    // 连接系统lirc服务
    int lirc_fd = lirc_init("lirc",1);
    if(lirc_fd == -1)
    {
        printf("lirc服务连接失败！请启动lircd服务\n");
        return -1;
    }
    printf("红外遥控硬件与LIRC客户端初始化完成\n");
    return 0;
}

/**
 * @brief 持续循环监听红外遥控按键，带400ms按键消抖
 */
void ir_remote_listen_loop(void)
{
    struct lirc_config *config;
    int buttonTimer = millis();
    char *code;

    // 读取lirc按键配置
    if(lirc_readconfig(NULL,&config,NULL) != 0)
    {
        printf("读取lirc配置文件失败\n");
        lirc_deinit();
        return;
    }

    printf("等待遥控器按键输入...\n");
    while(lirc_nextcode(&code)==0)
    {
        if(code == NULL)
            continue;
        // 按键消抖：间隔大于400ms才响应
        if (millis() - buttonTimer > 400)
        {
            ircontrol(code);
            buttonTimer = millis();
        }
        free(code);
    }

    // 释放资源
    lirc_freeconfig(config);
    lirc_deinit();
}

// 模块独立运行入口
int main(void)
{
    if(ir_remote_all_init() != 0)
        return 1;
    ir_remote_listen_loop();
    t_stop(0);
    return 0;
}