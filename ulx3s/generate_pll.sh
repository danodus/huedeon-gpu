#!/bin/sh

ecppll -i 25 -o 50 -f pll_main.v -n pll_main
ecppll -i 25 -o 125 --clkout1 25 -f pll_video.v -n pll_video

