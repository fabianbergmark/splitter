#!/bin/bash

declare return_val

source info.sh

tether () {
    unset return_val
    iface=$( pand -c "$1" -n 2>&1 |
             grep "connected" |
             cut -d' ' -f2 )
    return_val=$iface
}

routing_table () {
    unset return_val

    used_nums=()
    found=0

    while read table; do
        if [[ $table != \#* ]]; then
            num=$( echo $table |
                   cut -d' ' -f1 )
            alias=$( echo $table |
                     cut -d' ' -f2 )

            used_nums+=($num)

            if [[ $alias = $table_alias ]]; then
                found=1
                table_num=$num
                break
            fi

        fi
    done < /etc/iproute2/rt_tables

    if [ $found -eq 0 ]; then
        sorted_nums=( $( for each in ${used_nums[@]}; do
                             echo $each
                         done |
                         sort -n ) )
        glap=0
        last=${sorted_nums[0]}

        for next in ${sorted_nums[@]}; do
            diff=$((next - last))

            if [ $diff -gt 1 ]; then
                table_num=$((last + 1))
                glap=1
                break
            fi
            last=$next
        done

        if [ $glap -eq 0 ]; then
            table_num=$((last + 1))
        fi

        echo "$table_num $table_alias" >> /etc/iproute2/rt_tables
    fi

    return_val=$table_num
}

if [ ! "$#" -eq 1 ]; then
    echo "Provice a bluetooth MAC"
    exit -1;
fi

bdaddr=$1

tether $bdaddr
iface=$return_val


if [[ $iface != "" ]]; then

    ip link set "$iface" up
    systemctl start dhcpcd@"$iface"

    ip_info "$iface"

    ip_addr=${return_val['addr']}
    ip_net=${return_val['net']}
    ip_gw=${return_val['gw']}

    table_alias=$bdaddr
    routing_table $table_alias
    table_num=$return_val

    ip route flush table "$table_num"
    ip route add "$ip_net" dev "$iface" src "$ip_addr" table "$table_num"
    ip route add default via "$ip_gw" dev "$iface" table "$table_num"
    ip rule add from "$ip_addr" table "$table_num"
    ip rule add fwmark "$table_num" table "$table_num"

    ip link |
    grep -v "state DOWN" |
    grep "^[0-9]\+:" |
    cut -d':' -f2 |
    cut -d' ' -f2 |
    while read i; do

        if [[ "$i" = "$iface" ]]; then
            continue
        elif [[ "$i" = "lo" ]]; then
            i_net="127.0.0.0/8"
        else
            ip_info $i
            i_addr=${return_val['addr']}
            i_net=${return_val['net']}
            i_gw=${return_val['gw']}
        fi

        if [[ "$ip_net" == "$i_net" ]]; then
            continue
        fi

        ip route add "$i_net" dev "$i" table "$table_num"

    done

    iptables -A FORWARD -i "$iface" -m state --state ESTABLISHED,RELATED -j ACCEPT
    iptables -t nat -A POSTROUTING -o "$iface" -j MASQUERADE
    iptables -A FORWARD -i "$iface" -j REJECT

    iptables -t mangle --flush
    iptables -t mangle -A PREROUTING -j CONNMARK --restore-mark
    iptables -t mangle -A PREROUTING -m mark ! --mark 0 -j ACCEPT

    c=1

    for i in $( ip route |
                grep "^default" |
                cut -d' ' -f5 |
                sort -u ); do

        ip_info $i

        i_addr=${return_val['addr']}

        rt_info "$i_addr"

        i_table_alias=${return_val['alias']}
        i_table_num=${return_val['num']}

        prob=$( echo "1.0 / $c" | bc -l | head -c3 )

        if [[ "$prob" = "1.0" ]]; then
            iptables -t mangle -A PREROUTING -j MARK --set-mark "$i_table_num"
        else
            iptables -t mangle -A PREROUTING -m statistic --mode random --probability $prob -j MARK --set-mark "$i_table_num"
        fi
        ((c++))
    done

    iptables -t mangle -A PREROUTING -j CONNMARK --save-mark

    echo 0 > "/proc/sys/net/ipv4/conf/$iface/rp_filter"

    echo "$iface"
else
    echo "The device doesn't support bluetooth tethering"
    exit -1
fi
