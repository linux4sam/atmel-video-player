#!/bin/sh

sleep 2
dd if=/dev/zero of=/dev/fb0

cd "$(dirname "$0")"
./player -platform linuxfb:fb=/dev/fb1 -plugin tslib:/dev/input/event1 -plugin evdevkeyboard:/dev/input/event0 $1

dd if=/dev/zero of=/dev/fb1
