#!/bin/bash

ip_info () {
    unset return_val
    declare -Ag return_val

    ip_addr=$( ip addr show "$1" |
        grep "inet [0-9]\+\.[0-9]\+\.[0-9]\+" |
        cut -d' ' -f6 |
        cut -d'/' -f1 )
    ip_net=$( ip route |
        grep "$1" |
        grep "$ip_addr" |
        cut -d' ' -f1 )
    ip_gw=$( ip route |
        grep "default" |
        grep "$1" |
        cut -d' ' -f3 )

    return_val['addr']=$ip_addr
    return_val['net']=$ip_net
    return_val['gw']=$ip_gw
}

rt_info () {
    unset return_val
    declare -Ag return_val

    addr=$1
    rt_alias=$( ip rule show | grep "$addr" | cut -f2 | cut -d' ' -f4 | sort -u )
    rt_num=$( cat /etc/iproute2/rt_tables | grep "$rt_alias" | cut -d' ' -f1 )

    return_val['alias']=$rt_alias
    return_val['num']=$rt_num
}
