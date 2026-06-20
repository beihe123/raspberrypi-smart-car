#include <wiringPiI2C.h>
#include "pca9685.h"
#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE
#define LED0_ON_L 0x6
#define LEDALL_ON_L 0xFA
#define PIN_ALL 16

static void myPwmWrite(struct wiringPiNodeStruct *node, int pin, int value);
static void myOnOffWrite(struct wiringPiNodeStruct *node, int pin, int value);
static int myOffRead(struct wiringPiNodeStruct *node, int pin);
static int myOnRead(struct wiringPiNodeStruct *node, int pin);
int baseReg(int pin);

int pca9685Setup(const int pinBase, const int i2cAddress, float freq)
{
    struct wiringPiNodeStruct *node = wiringPiNewNode(pinBase, PIN_ALL + 1);
    if (!node) return -1;
    int fd = wiringPiI2CSetup(i2cAddress);
    if (fd < 0) return fd;
    int settings = wiringPiI2CReadReg8(fd, PCA9685_MODE1) & 0x7F;
    int autoInc = settings | 0x20;
    wiringPiI2CWriteReg8(fd, PCA9685_MODE1, autoInc);
    if (freq > 0) pca9685PWMFreq(fd, freq);
    node->fd = fd;
    node->pwmWrite = myPwmWrite;
    node->digitalWrite = myOnOffWrite;
    node->digitalRead = myOffRead;
    node->analogRead = myOnRead;
    return fd;
}

void pca9685PWMFreq(int fd, float freq)
{
    freq = (freq > 1000 ? 1000 : (freq < 40 ? 40 : freq));
    int prescale = (int)(25000000.0f / (4096 * freq) - 0.5f);
    int settings = wiringPiI2CReadReg8(fd, PCA9685_MODE1) & 0x7F;
    int sleep = settings | 0x10;
    int wake = settings & 0xEF;
    int restart = wake | 0x80;
    wiringPiI2CWriteReg8(fd, PCA9685_MODE1, sleep);
    wiringPiI2CWriteReg8(fd, PCA9685_PRESCALE, prescale);
    wiringPiI2CWriteReg8(fd, PCA9685_MODE1, wake);
    delay(1);
    wiringPiI2CWriteReg8(fd, PCA9685_MODE1, restart);
}

void pca9685PWMReset(int fd)
{
    wiringPiI2CWriteReg16(fd, LEDALL_ON_L, 0x0);
    wiringPiI2CWriteReg16(fd, LEDALL_ON_L + 2, 0x1000);
}

void pca9685PWMWrite(int fd, int pin, int on, int off)
{
    int reg = baseReg(pin);
    wiringPiI2CWriteReg16(fd, reg, on & 0x0FFF);
    wiringPiI2CWriteReg16(fd, reg + 2, off & 0x0FFF);
}

void pca9685PWMRead(int fd, int pin, int *on, int *off)
{
    int reg = baseReg(pin);
    if (on) *on = wiringPiI2CReadReg16(fd, reg);
    if (off) *off = wiringPiI2CReadReg16(fd, reg + 2);
}

void pca9685FullOn(int fd, int pin, int tf)
{
    int reg = baseReg(pin) + 1;
    int state = wiringPiI2CReadReg8(fd, reg);
    state = tf ? (state | 0x10) : (state & 0xEF);
    wiringPiI2CWriteReg8(fd, reg, state);
    if (tf) pca9685FullOff(fd, pin, 0);
}

void pca9685FullOff(int fd, int pin, int tf)
{
    int reg = baseReg(pin) + 3;
    int state = wiringPiI2CReadReg8(fd, reg);
    state = tf ? (state | 0x10) : (state & 0xEF);
    wiringPiI2CWriteReg8(fd, reg, state);
}

int baseReg(int pin)
{
    return (pin >= PIN_ALL ? LEDALL_ON_L : LED0_ON_L + 4 * pin);
}

static void myPwmWrite(struct wiringPiNodeStruct *node, int pin, int value)
{
    int fd = node->fd;
    int ipin = pin - node->pinBase;
    if (value >= 4096)
        pca9685FullOn(fd, ipin, 1);
    else if (value > 0)
        pca9685PWMWrite(fd, ipin, 0, value);
    else
        pca9685FullOff(fd, ipin, 1);
}

static void myOnOffWrite(struct wiringPiNodeStruct *node, int pin, int value)
{
    int fd = node->fd;
    int ipin = pin - node->pinBase;
    if (value) pca9685FullOn(fd, ipin, 1);
    else pca9685FullOff(fd, ipin, 1);
}

static int myOffRead(struct wiringPiNodeStruct *node, int pin)
{
    int fd = node->fd;
    int ipin = pin - node->pinBase;
    int off;
    pca9685PWMRead(fd, ipin, 0, &off);
    return off;
}

static int myOnRead(struct wiringPiNodeStruct *node, int pin)
{
    int fd = node->fd;
    int ipin = pin - node->pinBase;
    int on;
    pca9685PWMRead(fd, ipin, &on, 0);
    return on;
}