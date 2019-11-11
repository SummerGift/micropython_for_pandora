# MicroPython for Pandora IoT Board

![IoT_Board](docs/figures/IoT_Board.png)

IoT Board 潘多拉是 RT-Thread 推出的一款物联网开发板，它给开发者带来了物联网时代的无限可能。而现在，它已经不仅仅是一块简单的物联网开发板，因为它已经全面支持 MicroPython 。在 IoT Board 上，你将会体验到有别于传统的，前所未有的开发方式。

借助于 MicroPython，你将有能力使用 Python 语言控制所有硬件外设，体验高级语言带来的便利特性，与此同时你还可以利用高级软件库快速实现你的 IoT 构想。

## 硬件支持

Pandora MicroPython 固件硬件功能如下所示：

| 外设名称 | 引脚号                                         | 简介                                        |
| -------- | ---------------------------------------------- | ------------------------------------------- |
| pin      | PA4 PA8, PB8-9 PB10-15, PC2 PC4 PC6-7, PD12-15 | 开发板引出的，可自由分配的 IO，支持引脚中断 |
| led      | PE7                                            | 红色 led 灯                                 |
| rgb      | R: PE7, G: PE8, B: PE9                         | rgb 灯                                      |
| key      | KEY0: PD10, KEY1: PD9, KEY2: PD8               | 输入按键                                    |
| uart1    | PA9, PA10                                      | 串口1                                       |
| i2c      |                                                | 软件 i2c 可选择任意 pin                     |
| spi      |                                                | 软件 spi 可选择任意引出 pin                 |
| adc      | PC4                                            | adc1，通道 13                               |
| pwm      | PB0                                            | pwm3,  通道 3,  用于红外发射                |
| timer    |                                                | 硬件定时器 15                               |
| wdt      |                                                | 看门狗                                      |
| rtc      |                                                | 实时时钟                                    |
| beeper   | PB2                                            | 蜂鸣器                                      |
| lcd      |                                                | lcd 显示屏                                  |
| wifi     |                                                | wifi 网络连接                               |
| aht10    | CLK: PD6, SDA: PC1                             | 温湿度传感器                                |
| ap3216c  | CLK: PC0, SDA: PC1                             | 接近与光强传感器                            |
| icm20608 | CLK: PC0, SDA: PC1                             | 六轴传感器                                  |

## MicroPython 库

- [MicroPython Librarys](docs/micropython-librarys.md)

## RT-Thread MicroPython 开发环境

- [RT-Thread MicroPython develop environment](https://marketplace.visualstudio.com/items?itemName=RT-Thread.rt-thread-micropython)