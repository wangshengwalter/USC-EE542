#!/bin/bash

sudo rm -f server.c
sudo rm -f server

wget https://raw.githubusercontent.com/wangshengwalter/USC-EE542/refs/heads/pipline/Lab4/server.c
gcc -o server server.c

./server 10.0.3.122 4000

