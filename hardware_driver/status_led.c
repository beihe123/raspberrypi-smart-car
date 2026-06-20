/*
任务8：智能小车工作状态指示
知识点：LED单向导电性、硬件原理图分析、wiringPi库GPIO输出编程
硬件说明：
绿色LED D5 → BCM GPIO5 → wiringPi编号21
红色LED D6 → BCM GPIO6 → wiringPi编号22
电路：GPIO输出高电平LED点亮，低电平熄灭
*/
#include <wiringPi.h>
#include <stdio.h>

// 引脚宏定义 wiringPi编号
#define GREEN_LED  21
#define RED_LED    22

/**
 * @brief LED硬件初始化
 * 将红绿LED引脚配置为输出，上电默认熄灭
 */
void led_init(void)
{
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    // 上电全部熄灭
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
}

/**
 * @brief 全部LED点亮
 */
void led_all_on(void)
{
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, HIGH);
}

/**
 * @brief 全部LED熄灭
 */
void led_all_off(void)
{
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
}

/**
 * @brief 单独点亮绿灯
 */
void green_led_on(void)
{
    digitalWrite(GREEN_LED, HIGH);
}

/**
 * @brief 单独熄灭绿灯
 */
void green_led_off(void)
{
    digitalWrite(GREEN_LED, LOW);
}

/**
 * @brief 单独点亮红灯
 */
void red_led_on(void)
{
    digitalWrite(RED_LED, HIGH);
}

/**
 * @brief 单独熄灭红灯
 */
void red_led_off(void)
{
    digitalWrite(RED_LED, LOW);
}

/**
 * @brief 红绿交替闪烁（状态提示主逻辑）
 */
void led_blink_alt(void)
{
    green_led_on();
    red_led_off();
    delay(500);

    green_led_off();
    red_led_on();
    delay(500);
}

// 测试入口函数
int main(void)
{
    // wiringPi初始化，失败直接退出
    if (wiringPiSetup() == -1)
    {
        printf("wiringPi库初始化失败，请使用sudo运行程序\n");
        return -1;
    }

    led_init();
    printf("LED状态指示程序启动，红绿交替闪烁\n");

    // 嵌入式主死循环
    while (1)
    {
        led_blink_alt();
    }

    return 0;
}