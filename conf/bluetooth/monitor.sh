#!/bin/bash

declare -A ifaces

stdbuf -o0 -i0 bt-monitor |
stdbuf -o0 -i0 grep "Connected -> " |
while read line
do

    state=$( echo "$line" |
             sed 's/.*Connected -> \([0-1]\).*/\1/' )
    bdaddr=$( echo "$line" |
              cut -d"(" -f2 |
              cut -d")" -f1 )

    if [ $state -eq 1 ]; then

        paired=$( bt-device -i "$bdaddr" |
                  grep "Paired" |
                  cut -d' ' -f4 )
        if [ $paired -eq 1 ]; then
            iface=$( ./connect.sh "$bdaddr" )
            ifaces[$bdaddr]=$iface
        else
            echo "Not paried"
        fi
    elif [ $state -eq 0 ]; then
        if [ ${ifaces[$bdaddr]+_} ]; then
            iface=${ifaces[$bdaddr]}
            ./disconnect.sh "$bdaddr" "$iface"
        fi
    fi
done
