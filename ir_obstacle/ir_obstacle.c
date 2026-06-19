/*
模块：ir_obstacle 红外避障模块
任务15：红外避障硬件原理、LM393比较器电路、两路避障传感器硬件调试逻辑
任务16：读取避障传感器电平，调用basic_move运动函数实现全自动避障行驶
硬件电平规则：
无障碍物、红外正常反射 → DO输出LOW(0)
检测到障碍物、红外被遮挡无反射 → DO输出HIGH(1)
硬件引脚：左避障26、右避障27（wiringPi编号）
*/
#include "ir_obstacle.h"
#include <stdio.h>

/**
 * @brief 避障传感器单独初始化
 * 将两路红外避障DO引脚配置为输入模式
 */
static int ir_sensor_init(void)
{
    if(wiringPiSetup() == -1)
    {
        printf("wiringPi初始化失败，请使用sudo运行程序\n");
        return -1;
    }
    pinMode(IR_OBSTACLE_LEFT, INPUT);
    pinMode(IR_OBSTACLE_RIGHT, INPUT);
    printf("两路红外避障传感器初始化完成\n");
    return 0;
}

/**
 * @brief 整车全部硬件统一初始化
 * 避障传感器 + TB6612电机 + LED指示灯 + 蜂鸣报警 + 启停按键
 */
int ir_all_hard_init(void)
{
    if(ir_sensor_init() != 0)
        return -1;
    car_all_hardware_init();
    printf("红外避障小车全部硬件初始化完毕\n");
    return 0;
}

/**
 * @brief 读取左右两路避障探头电平值
 * @param sl 左探头状态输出
 * @param sr 右探头状态输出
 */
void ir_read_sensor(int *sl, int *sr)
{
    *sl = digitalRead(IR_OBSTACLE_LEFT);
    *sr = digitalRead(IR_OBSTACLE_RIGHT);
}

/**
 * @brief 任务16核心自动避障逻辑
 * @param speed 行驶速度 0~100
 * 逻辑严格匹配任务文档四路分支判断：
 * 1. SL=LOW SR=LOW 双侧无障碍 → t_up直行
 * 2. SL=LOW SR=HIGH 右侧障碍 → t_left左转避让
 * 3. SL=HIGH SR=LOW 左侧障碍 → t_right右转避让
 * 4. SL=HIGH SR=HIGH 双侧同时障碍 → t_down后退1s，再原地左转1s
 */
void ir_auto_obstacle_run(unsigned int speed)
{
    int sl, sr;
    while(1)
    {
        ir_read_sensor(&sl, &sr);
        printf("左避障:%d  右避障:%d\n", sl, sr);

        if(sl == LOW && sr == LOW)
        {
            // 两侧均无障碍物，正常直行
            printf("直行，无障碍物\n");
            t_up(speed, 0);
        }
        else if(sl == LOW && sr == HIGH)
        {
            // 右侧检测到障碍物，向左转向避让
            printf("右侧遇障，向左转弯\n");
            t_left(speed, 0);
            buzzer_short_warn(); // 障碍短促蜂鸣报警
        }
        else if(sl == HIGH && sr == LOW)
        {
            // 左侧检测到障碍物，向右转向避让
            printf("左侧遇障，向右转弯\n");
            t_right(speed, 0);
            buzzer_short_warn();
        }
        else if(sl == HIGH && sr == HIGH)
        {
            // 左右同时存在障碍物，先后退再左转
            printf("双侧遇障，后退避让\n");
            t_down(speed, 1000);
            t_left(speed, 1000);
        }
        delay(15); // 传感器电平软件消抖，滤除干扰抖动
    }
}

// 模块独立测试入口：按键一键启停自动避障程序
int main(void)
{
    if(ir_all_hard_init() != 0)
        return 1;
    printf("红外自动避障程序启动，按下按键开启避障行驶\n");

    int run_flag = 0;
    while(1)
    {
        // 按键10ms软件消抖检测
        if(digitalRead(KEY_PIN) == 1)
        {
            delay(10);
            if(digitalRead(KEY_PIN) == 1)
            {
                run_flag = !run_flag;
                while(digitalRead(KEY_PIN) == 1) delay(10);
                if(run_flag == 1)
                {
                    printf("按键触发，开启全自动红外避障\n");
                    ir_auto_obstacle_run(40);
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