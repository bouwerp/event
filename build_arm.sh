#!/bin/bash
# Example build for ARM 6 architecture, utilising crosstool-ng.

rm -f event.o libevent.a
/Volumes/toolchains/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-gcc -c event.c
/Volumes/toolchains/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-ar rvs libevent.a event.o
