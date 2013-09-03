#!/bin/bash

source info.sh

if [ "$#" -lt 1 ]; then
    echo "Provide a bluetooth MAC"
    exit -1;
fi

bdaddr=$1

rev_bdaddr=$( echo "$bdaddr" | rev | sed 's/\([0-9A-F]\)\([0-9A-F]\)/\2\1/g' )
iface=$( pand -l | grep "$rev_bdaddr" | cut -d' ' -f1 )

if [[ ! "$iface" = "" ]]; then
    pand -k "$bdaddr"
else
    if [ "$#" -lt 2 ]; then
        echo "Provide a bluetooth MAC and associated interface"
        exit -1;
    fi
    iface=$2
fi

iptables -t mangle --flush
iptables -t mangle -A PREROUTING -j CONNMARK --restore-mark
iptables -t mangle -A PREROUTING -m mark ! --mark 0 -j ACCEPT

i=1

for default_iface in $( ip route | grep "^default" | cut -d' ' -f5 | sort -u ); do

    ip_info "$default_iface"

    iface_addr=${return_val['addr']}

    rt_info "$iface_addr"

    iface_table_alias=${return_val['alias']}
    iface_table_num=${return_val['num']}

    prob=$( echo "1.0 / $i" | bc -l | head -c3 )

    if [[ "$prob" = "1.0" ]]; then
        iptables -t mangle -A PREROUTING -j MARK --set-mark "$iface_table_num"
    else
        iptables -t mangle -A PREROUTING -m statistic --mode random --probability "$prob" -j MARK --set-mark "$iface_table_num"
    fi
    ((i++))

done

iptables -t mangle -A PREROUTING -j CONNMARK --save-mark

iptables -D FORWARD -i "$iface" -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -t nat -D POSTROUTING -o "$iface" -j MASQUERADE
iptables -D FORWARD -i "$iface" -j REJECT

ip route flush table "$bdaddr"
while ip rule del table "$bdaddr" ; do :; done 2> /dev/null
