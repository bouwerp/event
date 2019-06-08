#!/bin/bash
# Example build for ARM 6 architecture, utilising crosstool-ng.

rm -f event.o libevent.a
/Volumes/crosstool/x-tools/armv6-rpi-linux-gnueabi/bin/armv6-rpi-linux-gnueabi-gcc -c event.c
/Volumes/crosstool/x-tools/armv6-rpi-linux-gnueabi/bin/armv6-rpi-linux-gnueabi-ar rvs libevent.a event.o
