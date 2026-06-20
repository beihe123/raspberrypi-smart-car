/*
任务9：工作状态报警指示
知识点：有源/无源蜂鸣器区分、电路原理图、wiringPi softTone音调输出
硬件说明：
无源蜂鸣器控制引脚 BCM17 → wiringPi编号0
三极管驱动，需要方波/不同频率音调驱动发声
依赖库：wiringPi.h softTone.h
编译需链接 -lwiringPi
*/
#include <wiringPi.h>
#include <softTone.h>
#include <stdio.h>

// 蜂鸣器wiringPi引脚定义
#define BUZ_PIN     0

// 音调频率定义 低音CL 中音CM 高音CH
#define CL1  131
#define CL2  147
#define CL3  165
#define CL4  175
#define CL5  196
#define CL6  221
#define CL7  248

#define CM1  262
#define CM2  294
#define CM3  330
#define CM4  350
#define CM5  393
#define CM6  441
#define CM7  495

#define CH1  525
#define CH2  589
#define CH3  661
#define CH4  700
#define CH5  786
#define CH6  882
#define CH7  990

// 曲目1 音调数组
int song_1[] = {CM3,CM5,CM6,CM3,CM2,CM3,CM5,CM6,CH1,CM6,CM5,CM1,CM3,CM2,
                CM2,CM3,CM5,CM2,CM3,CM3,CL6,CL6,CL6,CM1,CM2,CM3,CM2,CL7,
                CL6,CM1,CL5};
// 曲目1节拍 单位500ms
int beat_1[] = {1,1,3,1,1,3,1,1,1,1,1,1,1,1,3,1,1,3,1,1,1,1,1,1,1,2,1,1,
                1,1,1,1,1,1,3};

// 曲目2 音调数组
int song_2[] = {CM1,CM1,CM1,CL5,CM3,CM3,CM3,CM1,CM1,CM3,CM5,CM5,CM4,CM3,CM2,
                CM2,CM3,CM4,CM4,CM3,CM2,CM3,CM1,CM1,CM3,CM2,CL5,CL7,CM2,CM1};
int beat_2[] = {1,1,1,3,1,1,1,3,1,1,1,1,1,1,3,1,1,1,2,1,1,1,3,1,1,1,3,3,2,3};

// 摩斯电报滴答延时基础值
#define TICK_DELAY 70

/**
 * @brief 蜂鸣器硬件初始化
 * wiringPi初始化 + softTone音调引脚创建
 * return 0成功 -1失败
 */
int buzzer_init(void)
{
    if(wiringPiSetup() == -1)
    {
        printf("wiringPi初始化失败！请使用sudo运行程序\n");
        return -1;
    }
    if(softToneCreate(BUZ_PIN) == -1)
    {
        printf("softTone蜂鸣器引脚创建失败\n");
        return -1;
    }
    return 0;
}

/**
 * @brief 播放指定曲目
 * @param tone_arr 音调数组
 * @param beat_arr 节拍数组
 */
void buzzer_play_song(int tone_arr[], int beat_arr[])
{
    int len = sizeof(tone_arr) / sizeof(int);
    for(int i = 0; i < len; i++)
    {
        softToneWrite(BUZ_PIN, tone_arr[i]);
        delay(beat_arr[i] * 500);
    }
    softToneWrite(BUZ_PIN, 0); // 停止发声
}

/**
 * @brief 播放两首循环背景音乐
 */
void buzzer_loop_music(void)
{
    printf("开始播放曲目1\n");
    buzzer_play_song(song_1, beat_1);
    delay(1000);
    printf("开始播放曲目2\n");
    buzzer_play_song(song_2, beat_2);
    delay(1000);
}

/**
 * @brief 摩斯电报滴滴报警声（简易障碍物报警）
 */
void buzzer_morse_alarm(void)
{
    char *morse = "01 1000 1010 100 0 0010 110 0000";
    char *p = morse;
    pinMode(BUZ_PIN, OUTPUT); // 普通高低电平模式
    while(*p != '\0')
    {
        switch(*p)
        {
            case '0': // 短滴
                digitalWrite(BUZ_PIN, LOW);
                delay(TICK_DELAY);
                digitalWrite(BUZ_PIN, HIGH);
                delay(TICK_DELAY);
                break;
            case '1': // 长嗒
                digitalWrite(BUZ_PIN, LOW);
                delay(TICK_DELAY * 3);
                digitalWrite(BUZ_PIN, HIGH);
                delay(TICK_DELAY);
                break;
            case ' ': // 字符间隔
                delay(TICK_DELAY * 2);
                break;
        }
        p++;
    }
    softToneCreate(BUZ_PIN); // 切回音调模式
}

/**
 * @brief 小车避障短促报警提示音
 */
void buzzer_short_warn(void)
{
    softToneWrite(BUZ_PIN, CM7);
    delay(150);
    softToneWrite(BUZ_PIN, 0);
    delay(150);
    softToneWrite(BUZ_PIN, CM7);
    delay(150);
    softToneWrite(BUZ_PIN, 0);
}

// 测试主函数
int main(void)
{
    if(buzzer_init() != 0)
        return 1;
    printf("蜂鸣器报警程序启动\n");

    while(1)
    {
        // 循环播放音乐
        buzzer_loop_music();
        // 也可替换为电报报警
        // buzzer_morse_alarm();
        // 单次短促报警
        // buzzer_short_warn();
    }
    return 0;
}