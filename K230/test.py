'''
实验名称：快速线性回归（巡线）
实验平台：01Studio CanMV K230
教程：wiki.01studio.cc
说明：通过修改lcd_width和lcd_height参数值选择3.5寸或2.4寸mipi屏。
'''

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

'''
#2.4寸mipi屏分辨率定义
lcd_width = 640
lcd_height = 480
'''

THRESHOLD = (0, 100)  # 黑白图像的灰度阈值
BINARY_VISIBLE = False # 使用二值化图像你可以看到什么是线性回归。
                        # 这可能降低 FPS（每秒帧数）.

sensor = Sensor(width=1280, height=960) #构建摄像头对象，将摄像头长宽设置为4:3
sensor.reset() #复位和初始化摄像头
sensor.set_framesize(width=640, height=480) #设置帧大小，默认通道0
sensor.set_pixformat(Sensor.GRAYSCALE) #设置输出图像格式，默认通道0

Display.init(Display.ST7701,  width=lcd_width, height=lcd_height, to_ide=True) #同时使用mipi屏和IDE缓冲区显示图像
#Display.init(Display.VIRT, sensor.width(), sensor.height()) #只使用IDE缓冲区显示图像

MediaManager.init() #初始化media资源管理器

sensor.run() #启动sensor

clock = time.clock()

fpioa = FPIOA()
fpioa.set_function(3,FPIOA.UART1_TXD)
fpioa.set_function(4,FPIOA.UART1_RXD)
uart=UART(UART.UART1,115200) #设置串口号1和波特率


while True:

    clock.tick()
    uart.write('Hello 01Studio!\r\n')

    # 步骤 1: 拍照并进行“二值化”处理
    # a. 先用摄像头拍一张照片
    original_img = sensor.snapshot()

    # b. 将照片变成只有黑白的“简笔画”（二值化）
    #    这样可以突出赛道线条，方便电脑识别
    if BINARY_VISIBLE:
        # 如果 BINARY_VISIBLE 是 True, 就进行处理
        # THRESHOLD 是区分黑白的标准线
        img = original_img.binary([THRESHOLD])
    else:
        # 否则，直接使用原始的灰度图
        img = original_img


    # 步骤 2: 分析图像，找出赛道线
    # a. 设定要寻找的颜色
    if BINARY_VISIBLE:
        # 如果是黑白画，那我们就寻找白色(255)的像素点
        color_to_track = [(255, 255)]
    else:
        # 如果是灰度图，就寻找在 THRESHOLD 范围内的像素
        color_to_track = [THRESHOLD]

    # b. 使用“线性回归”方法，在图中找到最佳的代表直线
    line = img.get_regression(color_to_track)


    # 步骤 3: 如果找到了线，计算并打印角度
    if (line):

        # 为了方便观察，把找到的线在屏幕上画出来
        img.draw_line(line.line(), color = 127,thickness=4)

        # 从直线信息中，提取出它的倾斜角度(0-180度)
        theta = line.theta()

        # 将角度从 0-180 转换到 -90 至 +90
        # 这样正数代表向右偏，负数代表向左偏，更直观
        if theta > 90:
            theta -= 180

        # 打印最终的角度
        print(theta)

    #显示图片，仅用于LCD居中方式显示
    Display.show_image(img, x=round((lcd_width-sensor.width())/2),y=round((lcd_height-sensor.height())/2))


    #print("FPS %f, mag = %s" % (clock.fps(), str(line.magnitude()) if (line) else "N/A"))
