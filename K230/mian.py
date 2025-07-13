# Untitled - By: xinge - Fri Jul 11 2025

import sys

import time, os, sys

from media.sensor import * #导入sensor模块，使用摄像头相关接口
from media.display import * #导入display模块，使用display相关接口
from media.media import * #导入media模块，使用meida相关接口
from machine import UART
from machine import FPIOA
import time


#3.5寸mipi屏分辨率定义
lcd_width = 800
lcd_height = 480
THRESHOLD =  (0, 100)
BINARY_VISIBLE = True # 使用二值化图像你可以看到什么是线性回归。 这可能降低 FPS（每秒帧数）.

# 下面的阈值元组是用来识别 红、绿、蓝三种颜色，当然你也可以调整让识别变得更好。
thresholds = [(30, 100, 15, 127, 15, 127), # 红色阈值
              (30, 100, -64, -8, 50, 70), # 绿色阈值
              (0, 40, 0, 90, -128, -20)] # 蓝色阈值
class Camera:
    # 初始化摄像头
    def __init__(self, channel='normal'):
        self.sensor = Sensor(width=1280, height=960) #构建摄像头对象
        self.sensor.reset() #复位和初始化摄像头
        self.channel = channel

        if self.channel == 'normal':
            # 正常显示通道
            self.sensor.set_framesize(width=800,height=480) #设置帧大小800x480
            self.sensor.set_pixformat(Sensor.RGB565) #设置输出图像格式为彩色
        elif self.channel == 'edge_detection':
            # 边缘检测通道
            self.sensor.set_framesize(width=640, height=480) #设置帧大小320x240
            self.sensor.set_pixformat(Sensor.GRAYSCALE) #设置输出图像格式为灰度
        else:
            # 未知通道，使用默认配置
            print(f"警告：未知通道 '{self.channel}'。将使用 'normal' 配置。")
            self.channel = 'normal'
            self.sensor.set_framesize(width=800,height=480)
            self.sensor.set_pixformat(Sensor.RGB565)

        Display.init(Display.ST7701, width=lcd_width, height=lcd_height, to_ide=True) #通过01Studio 3.5寸mipi显示屏显示图像
        MediaManager.init() #初始化media资源管理器
        self.sensor.run() #启动sensor

    # 获取摄像头帧
    # 参数：无
    # 返回值：图像帧
    def get_frame(self):
        img = self.sensor.snapshot() #拍摄一张图
        Display.show_image(img) #显示图片
        return img
    # 巡线
    def line_detection(self,imgs):
        # 预处理
        if BINARY_VISIBLE:
            # 二值化
            img = imgs.binary([THRESHOLD])
            line = img.get_regression([(255,255)])

        if (line):
            img.draw_line(line.line(), color = 127,thickness=4)
            theta = line.theta()
            if theta > 90:
                theta -= 180
        else:
            theta = 0
            img = imgs
        return theta,img
# 串口
class Serial:
    # 初始化串口
    def __init__(self):
        self.fpioa = FPIOA()
        self.fpioa.set_function(3,FPIOA.UART1_TXD)
        self.fpioa.set_function(4,FPIOA.UART1_RXD)
        self.uart=UART(UART.UART1,115200) #设置串口号1和波特率
    # 发送数据
    def send(self,data):
        if isinstance(data, list):
            data = bytearray(data)
        self.uart.write(str(data))
    # 接收数据
    def receive(self):
        return self.uart.read()

if __name__ == "__main__":
    clock = time.clock()

    # 您可以在这里选择不同的通道: 'normal' 或 'edge_detection'
    # 'normal': 正常显示摄像头画面
    # 'edge_detection': 执行边缘检测
    TARGET_CHANNEL = 'edge_detection'
    camera = Camera(channel=TARGET_CHANNEL)
    serial = Serial()
    BINARY_VISIBLE = True
    while True:
        # 刷新帧率
        clock.tick()
#        serial.send('Hello 01Studio!\r\n')

        # 获取图像
        imgs = camera.sensor.snapshot()
        # 获取误差角度
        theta,img = camera.line_detection(imgs)
        # 发送误差角度
        int_theta = int(abs(theta))

        if theta >= 0:
            data_to_send = [0x00, int_theta]
        else:
            data_to_send = [0xee, int_theta]
        serial.send(data_to_send)

        # 打印误差角度
        print(f"Theta: {theta}, Sent: {data_to_send}")
        # 显示图像
        Display.show_image(img)
        # 打印帧率

