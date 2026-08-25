CC = gcc
SHELL := /bin/bash
.PHONY: build clean 


client:
	$(CC) brx.c utils.c bridge.c device.c -o brx 
	mkdir -p bin
	mv -f brx bin/

server: 
	$(CC) brxd.c utils.c bridge.c device.c -o brxd
	mkdir -p bin
	mv -f brxd bin/

clean:
	rm -rf bin


