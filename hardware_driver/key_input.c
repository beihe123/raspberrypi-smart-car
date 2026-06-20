/*
任务10：智能小车按键处理
知识点：独立按键原理、硬件原理图分析、软件消抖、GPIO输入读取、查询式按键检测
硬件说明：
按键引脚 BCM19 → wiringPi编号24
电路：内部下拉电阻，未按下=低电平(0)，按下=高电平(1)
配套LED：绿LED wiringPi21、红LED wiringPi22，按键切换红绿状态
消抖方案：软件延时10ms消抖
*/
#include <wiringPi.h>
#include <stdio.h>
#include <string.h>

// 引脚宏定义
#define KEY_PIN     24      // 按键wiringPi引脚
#define GREEN_LED   21      // 绿色状态灯
#define RED_LED     22      // 红色状态灯

/**
 * @brief LED颜色切换控制函数
 * @param color 传入"RED"/"GREEN"切换对应灯亮
 */
void led_ctrl(const char* color)
{
    // 配置LED为输出模式
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);

    if(strcmp(color, "RED") == 0)
    {
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);
    }
    else if(strcmp(color, "GREEN") == 0)
    {
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
    }
    else
    {
        printf("LED颜色参数错误！\n");
    }
}

/**
 * @brief 按键硬件初始化
 * 按键设为输入模式，上电默认绿灯常亮
 */
int key_hardware_init(void)
{
    // wiringPi初始化
    if(wiringPiSetup() == -1)
    {
        printf("wiringPi库初始化失败，请使用sudo运行程序\n");
        return -1;
    }
    // 按键引脚配置为输入
    pinMode(KEY_PIN, INPUT);
    // 上电默认点亮绿灯
    led_ctrl("GREEN");
    printf("按键检测程序初始化完成\n");
    return 0;
}

// 测试主函数
int main(void)
{
    if(key_hardware_init() != 0)
        return 1;

    while(1)
    {
        // 检测按键按下（低电平松开，高电平按下）
        if(digitalRead(KEY_PIN) == 1)
        {
            delay(10);  // 软件消抖延时10ms
            // 再次确认按键按下
            if(digitalRead(KEY_PIN) == 1)
            {
                led_ctrl("RED");
                printf("按键已按下，切换为红灯\n");
                // 等待按键松开再切换回绿灯
                while(digitalRead(KEY_PIN) == 1);
                delay(10); // 松开消抖
                led_ctrl("GREEN");
                printf("按键松开，恢复绿灯\n");
            }
        }
    }
    return 0;
}