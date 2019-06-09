#!/bin/bash
# Example build for ARM 6 architecture, utilising crosstool-ng.

rm -f event.o libevent.a
/Volumes/toolchains/armv6-rpi-linux-gnueabi/bin/armv6-rpi-linux-gnueabi-gcc -c event.c
/Volumes/toolchains/armv6-rpi-linux-gnueabi/bin/armv6-rpi-linux-gnueabi-ar rvs libevent.a event.o
