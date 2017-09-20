#!/bin/sh

# Requires libnss3, libnss3-dev, libnspr4-dev


g++ -o main --std=c++14 -I/usr/include/nspr/ -lnss3 b64.cpp main.cpp