#!/bin/sh

set -x
# set -e

rmmod -f mydev
insmod mydev.ko

./writer RAY &
./reader 192.168.0.100 8888 /dev/mydev
