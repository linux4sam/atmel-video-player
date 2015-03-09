#!/bin/sh
sleep 1s

# clean up the LCD base layer, we will play video.
dd if=/dev/zero of=/dev/fb0

cd "$(dirname "$0")"
./player -platform linuxfb:fb=/dev/fb1 -plugin tslib:/dev/input/event1 -plugin evdevkeyboard:/dev/input/event0 $1 2>/dev/null

# we need clean the overlay before we quit video player
dd if=/dev/zero of=/dev/fb1
