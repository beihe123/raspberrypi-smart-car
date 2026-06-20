#!/usr/bin/env python3
import RPi.GPIO as GPIO
import time

# BOARD物理引脚定义，与硬件统一
PA = 12
A1 = 13
A2 = 15
PB = 16
B1 = 18
B2 = 22

pwm_ENA = None
pwm_ENB = None

def init():
    global pwm_ENA, pwm_ENB
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(PA, GPIO.OUT)
    GPIO.setup(A1, GPIO.OUT)
    GPIO.setup(A2, GPIO.OUT)
    GPIO.setup(PB, GPIO.OUT)
    GPIO.setup(B1, GPIO.OUT)
    GPIO.setup(B2, GPIO.OUT)
    pwm_ENA = GPIO.PWM(PA, 2000)
    pwm_ENB = GPIO.PWM(PB, 2000)
    pwm_ENA.start(0)
    pwm_ENB.start(0)

# 前进
def up(tf):
    init()
    GPIO.output(A1, GPIO.LOW)
    GPIO.output(A2, GPIO.HIGH)
    GPIO.output(B1, GPIO.LOW)
    GPIO.output(B2, GPIO.HIGH)
    pwm_ENA.ChangeDutyCycle(80)
    pwm_ENB.ChangeDutyCycle(80)
    time.sleep(tf)
    GPIO.cleanup()

# 后退
def down(tf):
    init()
    GPIO.output(A1, GPIO.HIGH)
    GPIO.output(A2, GPIO.LOW)
    GPIO.output(B1, GPIO.HIGH)
    GPIO.output(B2, GPIO.LOW)
    pwm_ENA.ChangeDutyCycle(60)
    pwm_ENB.ChangeDutyCycle(60)
    time.sleep(tf)
    GPIO.cleanup()

# 右转
def right(tf):
    init()
    GPIO.output(A1, GPIO.LOW)
    GPIO.output(A2, GPIO.HIGH)
    GPIO.output(B1, GPIO.LOW)
    GPIO.output(B2, GPIO.LOW)
    pwm_ENA.ChangeDutyCycle(60)
    pwm_ENB.ChangeDutyCycle(60)
    time.sleep(tf)
    GPIO.cleanup()

# 左转
def left(tf):
    init()
    GPIO.output(A1, GPIO.LOW)
    GPIO.output(A2, GPIO.LOW)
    GPIO.output(B1, GPIO.LOW)
    GPIO.output(B2, GPIO.HIGH)
    pwm_ENA.ChangeDutyCycle(60)
    pwm_ENB.ChangeDutyCycle(60)
    time.sleep(tf)
    GPIO.cleanup()

# 左前
def leftup(tf):
    init()
    GPIO.output(A1, GPIO.LOW)
    GPIO.output(A2, GPIO.HIGH)
    GPIO.output(B1, GPIO.LOW)
    GPIO.output(B2, GPIO.HIGH)
    pwm_ENA.ChangeDutyCycle(30)
    pwm_ENB.ChangeDutyCycle(60)
    time.sleep(tf)
    GPIO.cleanup()

# 右前
def rightup(tf):
    init()
    GPIO.output(A1, GPIO.LOW)
    GPIO.output(A2, GPIO.HIGH)
    GPIO.output(B1, GPIO.LOW)
    GPIO.output(B2, GPIO.HIGH)
    pwm_ENA.ChangeDutyCycle(60)
    pwm_ENB.ChangeDutyCycle(30)
    time.sleep(tf)
    GPIO.cleanup()

# 左后
def leftdown(tf):
    init()
    GPIO.output(A1, GPIO.HIGH)
    GPIO.output(A2, GPIO.LOW)
    GPIO.output(B1, GPIO.HIGH)
    GPIO.output(B2, GPIO.LOW)
    pwm_ENA.ChangeDutyCycle(30)
    pwm_ENB.ChangeDutyCycle(60)
    time.sleep(tf)
    GPIO.cleanup()

# 右后
def rightdown(tf):
    init()
    GPIO.output(A1, GPIO.HIGH)
    GPIO.output(A2, GPIO.LOW)
    GPIO.output(B1, GPIO.HIGH)
    GPIO.output(B2, GPIO.LOW)
    pwm_ENA.ChangeDutyCycle(60)
    pwm_ENB.ChangeDutyCycle(30)
    time.sleep(tf)
    GPIO.cleanup()

# 原地掉头
def turn_round(tf):
    init()
    GPIO.output(A1, GPIO.LOW)
    GPIO.output(A2, GPIO.HIGH)
    GPIO.output(B1, GPIO.HIGH)
    GPIO.output(B2, GPIO.LOW)
    pwm_ENA.ChangeDutyCycle(30)
    pwm_ENB.ChangeDutyCycle(30)
    time.sleep(tf)
    GPIO.cleanup()