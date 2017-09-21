#!/bin/sh

# Requires libnss3, libnss3-dev, libnspr4-dev, libsqlite3-0, libsqlite3-dev


g++ -o main --std=c++14 -I/usr/include/nspr/ -g -lnss3 -lsqlite3 b64.cpp firefox.cpp main.cpp