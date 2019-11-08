# MicroPython for Pandora IoT Board 

![IoT_Board](docs/figures/IoT_Board.png)

IoT Board 潘多拉是 RT-Thread 推出的一款物联网开发板，它给开发者带来了物联网时代的无限可能。而现在，它已经不仅仅是一块简单的物联网开发板，因为它已经全面支持 MicroPython 。在 IoT Board 上，你将会体验到有别于传统的，前所未有的开发方式。

借助于 MicroPython，你将有能力使用 Python 语言控制所有硬件外设，体验高级语言带来的便利特性，与此同时你还可以利用高级软件库快速实现你的 IoT 设想。

## 功能简介

MicroPython 固件硬件支持情况如下所示：

| 外设名称 | 引脚号 | 简介 |
| -------- | ------ | ---- |
| pin      |        |      |
| led      |        |      |
| key      |        |      |
| uart     |        |      |
| i2c      |        |      |
| spi      |        |      |
| adc      |        |      |
| pwm      |        |      |
| timer    |        |      |
| wdt      |        |      |
| beeper   |        |      |
| lcd      |        |      |
| wifi     |        |      |

## IoT Board machine 类介绍

- class ADC – analog to digital conversion
- class I2C – a two-wire serial protocol
- class LCD – LCD control for the LCD touch-sensor pyskin
- class LED – LED object
- class Pin – control I/O pins
- class RTC – real time clock
- class SPI – a master-driven serial protocol
- class Timer – control internal timers
- class TimerChannel — setup a channel for a timer
- class UART – duplex serial communication bus

## 开发资源介绍

- RT-Thread MicroPython 开发环境