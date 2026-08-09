CC = gcc
SHELL := /bin/bash
.PHONY: build clean 


build:
	$(CC) brx.c -o brx 
	mkdir -p bin
	mv -f brx bin/

clean:
	rm -rf bin

