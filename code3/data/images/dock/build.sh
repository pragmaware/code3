#!/bin/bash

ICONS=$(find src/ -name "*.*")
FORMATS="12"

for a in $ICONS; do
	for b in $FORMATS; do
		DST=$(echo $a | sed -e 's/src//g')
		echo building $a format ${b}x${b} in ./${b}px$DST
		convert -scale ${b}x${b} $a ./${b}px$DST
	done
done
