#!/bin/bash

sudo rm -f client.c
sudo rm -f client

wget https://raw.githubusercontent.com/wangshengwalter/USC-EE542/refs/heads/pipline/Lab4/client.c
gcc -o client client.c

./client filesystem.bin 10.0.3.122 4000
