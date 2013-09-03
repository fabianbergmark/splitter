#!/bin/bash

scan=$( hcitool scan | grep ":" | cut -f2 )

paired=$( bt-device --list | grep "(" | cut -d "(" -f2 | cut -d ")" -f1 )

for dev in $scan; do
    found=0
    for pair in $paired; do
        if [[ "$dev" == "$pair" ]]; then
            found=1
        fi
    done
    if [ $found -eq 0 ]; then
        echo "New device $dev"
        echo "yes" | bluez-simple-agent hci0 "$dev" > /dev/null 2>&1
        if [ $? -eq 1 ]; then
            echo "Paired with $dev"
        fi
    fi
done
