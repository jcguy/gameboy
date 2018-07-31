#!/usr/bin/env python3
for i in range(0x100):
    print("{\"NOP\", 0, NULL},\t\t", "// 0x{:02x}".format(i))
