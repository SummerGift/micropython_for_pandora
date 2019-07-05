# 
# Copyright (c) 2006-2019, RT-Thread Development Team
# 
# SPDX-License-Identifier: MIT License
# 
# Change Logs:
# Date           Author       Notes
# 2019-06-13     SummerGift   first version
#

import _thread
import utime as time

def testThread():
    while True:
        print("Hello from thread")
        time.sleep(2)

_thread.start_new_thread(testThread, ())
while True:
    pass
