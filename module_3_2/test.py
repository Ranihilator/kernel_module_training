#!/usr/bin/env python3

import os


fd = os.open("/dev/solution_node", os.O_RDWR | os.O_NOCTTY)
test = open(fd, "wb+", buffering=0)
test.write(b"Hello")
data = test.read()
print(data)
print('\n')
#data = test.read()
#print(data)
