#!/bin/bash

PATH=/usr/sbin:/sbin:/bin:/usr/bin

iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -A INPUT -m state --state NEW -i eth0 -j ACCEPT
iptables -A INPUT -m state --state NEW -i wlan0 -j ACCEPT

iptables -A FORWARD -i eth0 -j ACCEPT

iptables -A FORWARD -i wlan0 -j ACCEPT
