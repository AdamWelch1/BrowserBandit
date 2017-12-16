CC=g++
CFLAGS=--std=c++14 -I/usr/include/nspr/ -g -lnss3 -lsqlite3 -o bbandit

all:
	$(CC) $(CFLAGS) b64.cpp firefox.cpp main.cpp