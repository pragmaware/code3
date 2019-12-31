#!/bin/bash

ICONS="icon"
FORMATS="256 128 64 48 32 24 20 16"

for a in $ICONS; do
	for b in $FORMATS; do
		echo building icon $a format ${b}x${b}
		convert -scale ${b}x${b} src/$a.png ./icon${b}.png
	done
done
