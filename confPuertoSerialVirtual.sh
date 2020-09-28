#!/bin/bash
#Creacion de puertos seriales virtuales
sudo socat -d -d -d -v -x PTY,link=/dev/tty0,mode=777,unlink-close,raw,echo=0 PTY,link=/dev/tty1,mode=777,unlink-close,raw,echo=0
