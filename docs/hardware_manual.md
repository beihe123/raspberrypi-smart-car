# 智能小车整机硬件手册
## 一、硬件清单
1. 树莓派主板
2. TB6612电机驱动板（左右直流减速电机）
3. PCA9685 I2C舵机驱动板
4. HC-SR04超声波测距模块
5. 一体化红外接收头（LIRC红外遥控）
6. USB摄像头（mjpg-streamer视频）
7. 锂电池供电模块
8. 手机WiFi局域网控制

## 二、WiringPi BOARD物理引脚总定义（全项目统一，所有代码共用）
### 1）TB6612直流电机引脚
| 引脚(BOARD) | 功能       | 代码变量名 | 模块使用位置                     |
|------------|------------|------------|----------------------------------|
| 12         | PWMA左电机PWM | PWMA       | basic_move、wifi_network、app_control |
| 13         | AIN2左电机方向2 | AIN2     | 全部电机控制代码                 |
| 15         | AIN1左电机方向1 | AIN1     | 全部电机控制代码                 |
| 16         | PWMB右电机PWM | PWMB       | 全部电机控制代码                 |
| 18         | BIN2右电机方向2 | BIN2     | 全部电机控制代码                 |
| 22         | BIN1右电机方向1 | BIN1     | 全部电机控制代码                 |

### 2）HC-SR04超声波引脚（ultrasonic模块）
| 引脚(BOARD) | 功能  | 宏定义名  |
|------------|-------|-----------|
| 28         | Trig  | ULTRA_TRIG |
| 29         | Echo  | ULTRA_ECHO |

### 3）PCA9685 I2C舵机总线（I2C专用引脚，固定）
- I2C SDA：BOARD27
- I2C SCL：BOARD28
- PCA9685 I2C设备地址：0x40
- 舵机通道分配：
  - CH0：HC-SR04超声波摇头舵机（ultrasonic模块）
  - CH1：摄像头云台左右旋转（wifi_network、任务25）
  - CH2：摄像头云台上下俯仰（wifi_network、任务25）
- 舵机参数：50Hz PWM，0.5~2.5ms脉冲对应0~180°

### 4）红外接收头（lirc红外遥控 ir_remote）
硬件：红外接收头接树莓派默认LIRC接收引脚（通用BCM18/BOARD12，系统lircd.conf绑定）

### 5）USB摄像头
直接插入USB口，mjpg-streamer视频流，无GPIO引脚占用。

## 三、各模块功能说明
### 1. basic_move 基础电机驱动
提供统一接口 t_up/t_down/t_left/t_right/t_stop，所有控制模块（WiFi、WebAPP、红外遥控、超声波避障）全部复用，统一速度参数0~100。

### 2. ir_remote 红外遥控（任务21、22）
依赖lirc_client系统库，21组遥控器按键映射，5个核心按键控制小车运动，400ms按键消抖。

### 3. ultrasonic 超声波+舵机（任务17~20）
1. HC-SR04测距公式：距离(cm) = 34990 * 时间差 / 2
2. 舵机三方位扫描：5°右前 / 90°正前 / 175°左前
3. 障碍物危险阈值：40cm，遇障后退+择优转向

### 4. wifi_network TCP网络服务（任务23~25）
1. TCP多路非阻塞select并发服务，监听端口默认2001
2. 两套通信协议：
   - PC上位机：3字节 `ON+动作码(A/B/C/D/E/L/I/K/J)`
   - 手机APP：6字节数据包，偏移2存放指令
3. 扩展云台舵机远程控制（左右上下）
4. 配套mjpg-streamer实时视频传输

### 5. app_control Web网页APP（任务26）
1. Python3 + Bottle轻量Web框架，监听8080端口
2. 移动端触控HTML页面，支持8方向移动+原地掉头
3. 长按持续发送指令，松开自动停车
4. 局域网手机浏览器直接访问：树莓派IP:8080

## 四、硬件接线与供电注意事项
1. TB6612、PCA9685舵机板必须外接锂电池供电，不可仅靠树莓派5V；
2. I2C设备PCA9685需开启树莓派I2C功能：`raspi-config`；
3. HC-SR04 Echo引脚建议串联220Ω电阻保护GPIO；
4. 电机运行大电流时，树莓派易电压跌落，务必使用稳压锂电池；
5. 红外接收头避开电机PWM电磁干扰，远离驱动板。

## 五、编译与启动通用说明
1. C语言模块编译统一脚本：`base_env/run_car.sh`
2. Python网页模块：进入`app_control`执行`bash install.sh`安装依赖，`python3 web_server.py`启动
3. 红外遥控前置服务：`sudo systemctl start lircd`
4. 视频流启动：`cd mjpg-streamer/mjpg-streamer-experimental && sudo ./start.sh`