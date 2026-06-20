/*
模块：tracking_line 红外黑线循迹
任务13：红外循迹硬件原理、LM393模块电路分析、两路传感器硬件调试逻辑
任务14：读取红外传感器电平，结合basic_move运动函数实现自动循迹行驶
硬件说明：
两路数字红外循迹模块，VCC5V/GND接地，SIG输出wiringPi23(左)、25(右)
电平规则：
白线反射强 → SIG输出LOW(0)
黑线吸收红外 → SIG输出HIGH(1)
*/
#include "tracking_line.h"
#include <stdio.h>

/**
 * @brief 循迹传感器硬件初始化
 * 配置两路探头GPIO为输入模式
 */
static int track_sensor_init(void)
{
    if(wiringPiSetup() == -1)
    {
        printf("wiringPi初始化失败，请使用sudo运行程序\n");
        return -1;
    }
    // 红外探头设为输入
    pinMode(TRACK_LEFT, INPUT);
    pinMode(TRACK_RIGHT, INPUT);
    printf("两路红外循迹传感器初始化完成\n");
    return 0;
}

// 整车全套硬件初始化：循迹传感器+电机+LED+蜂鸣+按键
int track_all_hard_init(void)
{
    if(track_sensor_init() != 0)
        return -1;
    // 复用基础运动模块整车硬件初始化（电机、指示灯、蜂鸣、按键）
    car_all_hardware_init();
    printf("循迹小车全部硬件初始化完毕\n");
    return 0;
}

/**
 * @brief 读取左右两路红外探头电平
 * @param sl 左探头状态输出
 * @param sr 右探头状态输出
 */
void track_read_sensor(int *sl, int *sr)
{
    *sl = digitalRead(TRACK_LEFT);
    *sr = digitalRead(TRACK_RIGHT);
}

/**
 * @brief 任务14核心：无限自动循迹行驶
 * @param speed 行驶速度0~100
 * 逻辑匹配任务文档两路循迹规则：
 * 1. SL=LOW SR=LOW 双白线 → t_up直行
 * 2. SL=HIGH SR=LOW 左压黑线 → t_right向右修正
 * 3. SR=HIGH SL=LOW 右压黑线 → t_left向左修正
 * 4. 全黑线/异常 → t_stop停车
 */
void track_auto_run(unsigned int speed)
{
    int sl, sr;
    while(1)
    {
        track_read_sensor(&sl, &sr);
        // 打印探头状态，方便任务13硬件调试
        printf("左探头:%d  右探头:%d\n", sl, sr);

        if(sl == LOW && sr == LOW)
        {
            // 双探头均为白线，居中直行
            printf("直行\n");
            t_up(speed, 0);
        }
        else if(sl == HIGH && sr == LOW)
        {
            // 左探头压黑线，车身左偏，向右微调
            printf("右修正\n");
            t_right(speed, 0);
        }
        else if(sr == HIGH && sl == LOW)
        {
            // 右探头压黑线，车身右偏，向左微调
            printf("左修正\n");
            t_left(speed, 0);
        }
        else
        {
            // 双探头同时检测黑线，赛道丢失，停车
            printf("赛道丢失，停止\n");
            t_stop(0);
        }
        delay(10); // 软件消抖，滤除传感器电平抖动
    }
}

// 独立测试入口：按键一键启动自动循迹
int main(void)
{
    if(track_all_hard_init() != 0)
        return 1;
    printf("红外循迹程序启动，按下按键开始自动循迹\n");

    int run_flag = 0;
    while(1)
    {
        // 按键消抖检测
        if(digitalRead(KEY_PIN) == 1)
        {
            delay(10);
            if(digitalRead(KEY_PIN) == 1)
            {
                run_flag = !run_flag;
                while(digitalRead(KEY_PIN) == 1) delay(10);
                if(run_flag == 1)
                {
                    printf("按键触发，开启自动循迹\n");
                    track_auto_run(45);
                }
                else
                {
                    printf("按键关闭，小车停止\n");
                    t_stop(0);
                }
            }
        }
    }
    return 0;
}