/*
模块：wifi_network WiFi TCP远程控制服务端
任务23：网络编程基础，OSI/TCPIP模型、TCP/UDP区别、socket API、select非阻塞多路IO
任务24：TCP服务端，解析PC/手机双协议控制小车前进后退左右停
任务25：扩展云台舵机指令，远程控制摄像头左右上下摇头
硬件依赖：
1. TB6612电机驱动（复用basic_move）
2. PCA9685 I2C舵机驱动（复用ultrasonic/pca9685）
网络说明：
1. TCP流式socket，select实现非阻塞多路客户端并发
2. 两套通信协议：
   - PC上位机：数据包长度3，头ON + 动作码(A/B/C/D/E/L/I/K/J)
   - 手机APP：数据包长度6，偏移2位置存放动作字节
3. 内置mjpg-streamer视频配套，开启服务后可同时传输画面+控制指令
*/
#include "wifi_server.h"

// 云台全局角度初始居中
float g_lr_angle = 90.0f;
float g_ud_angle = 90.0f;

// 转换角度输出PCA9685 PWM
static int calcTicks(float impulseMs, int hertz)
{
    float cycleMs = 1000.0f / hertz;
    return (int)(4096 * impulseMs / cycleMs + 0.5f);
}

static void servo_write(int ch, float angle)
{
    float pulse = angle / 90.0f + 0.5f;
    if(pulse < 0.5f) pulse = 0.5f;
    if(pulse > 2.5f) pulse = 2.5f;
    int tick = calcTicks(pulse, 50);
    pwmWrite(300 + ch, tick);
}

// 云台左右旋转
void ptz_set_lr(float angle)
{
    g_lr_angle = angle;
    if(g_lr_angle < PTZ_MIN_ANG) g_lr_angle = PTZ_MIN_ANG;
    if(g_lr_angle > PTZ_MAX_ANG) g_lr_angle = PTZ_MAX_ANG;
    servo_write(PTZ_LR_CH, g_lr_angle);
}

// 云台上下旋转
void ptz_set_ud(float angle)
{
    g_ud_angle = angle;
    if(g_ud_angle < PTZ_MIN_ANG) g_ud_angle = PTZ_MIN_ANG;
    if(g_ud_angle > PTZ_MAX_ANG) g_ud_angle = PTZ_MAX_ANG;
    servo_write(PTZ_UD_CH, g_ud_angle);
}

// 整套硬件初始化：电机+PCA9685云台
int wifi_all_hard_init(void)
{
    if(wiringPiSetup() == -1)
    {
        printf("wiringPi初始化失败\n");
        return -1;
    }
    // 电机底层初始化（复用基础运动模块）
    car_all_hardware_init();
    // PCA9685云台初始化，I2C地址0x40 50Hz
    int pca_fd = pca9685Setup(300, 0x40, 50);
    if(pca_fd < 0)
    {
        printf("PCA9685云台初始化失败，请检查I2C接线\n");
        return -1;
    }
    pca9685PWMReset(pca_fd);
    // 云台上电归中
    ptz_set_lr(g_lr_angle);
    ptz_set_ud(g_ud_angle);
    printf("电机+摄像头云台硬件初始化完成\n");
    return 0;
}

// 解析网络数据包，区分PC 3字节 / 手机6字节协议，同时支持小车+云台指令
void parse_command(char *buf, int len)
{
    if(len == 3)
    {
        // PC上位机协议 ON + 动作码
        if(buf[0] == PC_HEAD1 && buf[1] == PC_HEAD2)
        {
            switch(buf[2])
            {
                case PC_FORWARD:
                    printf("WiFi指令：小车前进\n");
                    t_up(50, 0);
                    break;
                case PC_BACK:
                    printf("WiFi指令：小车后退\n");
                    t_down(50, 0);
                    break;
                case PC_LEFT:
                    printf("WiFi指令：小车左转\n");
                    t_left(50, 0);
                    break;
                case PC_RIGHT:
                    printf("WiFi指令：小车右转\n");
                    t_right(50, 0);
                    break;
                case PC_STOP:
                    printf("WiFi指令：小车停止\n");
                    t_stop(0);
                    break;
                // 云台扩展指令 任务25
                case PTZ_LEFT:
                    g_lr_angle += PTZ_STEP;
                    ptz_set_lr(g_lr_angle);
                    printf("云台向左转，当前角度：%.1f\n", g_lr_angle);
                    break;
                case PTZ_RIGHT:
                    g_lr_angle -= PTZ_STEP;
                    ptz_set_lr(g_lr_angle);
                    printf("云台向右转，当前角度：%.1f\n", g_lr_angle);
                    break;
                case PTZ_UP:
                    g_ud_angle += PTZ_STEP;
                    ptz_set_ud(g_ud_angle);
                    printf("云台上仰，当前角度：%.1f\n", g_ud_angle);
                    break;
                case PTZ_DOWN:
                    g_ud_angle -= PTZ_STEP;
                    ptz_set_ud(g_ud_angle);
                    printf("云台下俯，当前角度：%.1f\n", g_ud_angle);
                    break;
                default:
                    t_stop(0);
                    break;
            }
        }
        else
        {
            t_stop(0);
        }
    }
    else if(len == 6)
    {
        // 手机APP6字节协议
        unsigned char cmd = buf[MOBILE_HEAD_OFFSET];
        switch(cmd)
        {
            case MOBILE_FORWARD:
                printf("手机指令：前进\n");
                t_up(50, 0);
                break;
            case MOBILE_BACK:
                printf("手机指令：后退\n");
                t_down(50, 0);
                break;
            case MOBILE_LEFT:
                printf("手机指令：左转\n");
                t_left(50, 0);
                break;
            case MOBILE_RIGHT:
                printf("手机指令：右转\n");
                t_right(50, 0);
                break;
            case MOBILE_STOP:
                printf("手机指令：停止\n");
                t_stop(0);
                break;
            default:
                t_stop(0);
                break;
        }
    }
    else
    {
        // 非法长度数据包，停车
        t_stop(0);
    }
}

// TCP非阻塞多路select服务主循环
void wifi_tcp_server_loop(int port)
{
    int listenfd;
    int connectfd, sockfd;
    int ret, maxfd = -1, maxi = -1;
    struct timeval tv;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    char buf[BUFSIZE];
    int recv_len, i;
    fd_set rset, allset;
    CLIENT client[MAX_CLIENTS];

    // 初始化客户端数组全部置-1（空闲）
    for(i = 0; i < MAX_CLIENTS; i++)
        client[i].fd = -1;

    // 创建TCP socket
    listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if(listenfd < 0)
    {
        perror("socket create fail");
        return;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // 绑定端口
    if(bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind fail");
        close(listenfd);
        return;
    }
    // 监听队列128
    if(listen(listenfd, 128) < 0)
    {
        perror("listen fail");
        close(listenfd);
        return;
    }

    // 初始化文件描述符集合
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    maxfd = listenfd;
    tv.tv_sec = 0;
    tv.tv_usec = 1;

    printf("WiFi TCP服务启动成功，监听端口：%d\n", port);
    printf("配套视频流启动命令：cd mjpg-streamer/mjpg-streamer-experimental && sudo ./start.sh\n");

    while(1)
    {
        rset = allset;
        ret = select(maxfd + 1, &rset, NULL, NULL, &tv);
        if(ret == 0)
            continue;
        if(ret < 0)
        {
            perror("select error");
            break;
        }

        // 处理新客户端连接
        if(FD_ISSET(listenfd, &rset))
        {
            addr_len = sizeof(client_addr);
            connectfd = accept(listenfd, (struct sockaddr *)&client_addr, &addr_len);
            if(connectfd < 0)
                continue;
            // 存入空闲客户端槽位
            for(i = 0; i < MAX_CLIENTS; i++)
            {
                if(client[i].fd < 0)
                {
                    client[i].fd = connectfd;
                    client[i].addr = client_addr;
                    printf("新客户端接入：%s\n", inet_ntoa(client_addr.sin_addr));
                    break;
                }
            }
            if(i == MAX_CLIENTS)
                printf("客户端连接数达到上限\n");
            FD_SET(connectfd, &allset);
            if(connectfd > maxfd) maxfd = connectfd;
            if(i > maxi) maxi = i;
        }
        // 处理已有客户端数据
        else
        {
            for(i = 0; i <= maxi; i++)
            {
                sockfd = client[i].fd;
                if(sockfd < 0) continue;
                if(FD_ISSET(sockfd, &rset))
                {
                    bzero(buf, BUFSIZE);
                    recv_len = read(sockfd, buf, BUFSIZE);
                    if(recv_len > 0)
                    {
                        printf("收到数据包长度：%d 数据：%s\n", recv_len, buf);
                        parse_command(buf, recv_len);
                    }
                    else
                    {
                        // 客户端断开
                        printf("客户端 %s 断开连接\n", inet_ntoa(client[i].addr.sin_addr));
                        close(sockfd);
                        FD_CLR(sockfd, &allset);
                        client[i].fd = -1;
                    }
                }
            }
        }
    }
    close(listenfd);
}

// 程序独立运行入口
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("使用方法：sudo ./wifi_server 端口号\n示例：sudo ./wifi_server 2001\n");
        return 1;
    }
    int port = atoi(argv[1]);
    if(port <= 0 || port > 65535)
    {
        printf("端口号范围1~65535\n");
        return 1;
    }
    if(wifi_all_hard_init() != 0)
        return 1;
    wifi_tcp_server_loop(port);
    t_stop(0);
    return 0;
}