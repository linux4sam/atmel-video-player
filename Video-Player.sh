#!/bin/sh

# close stdout, stderr, stdin and double fork
((/opt/VideoPlayer/Player.sh >&- 2>&- <&- &)&)
