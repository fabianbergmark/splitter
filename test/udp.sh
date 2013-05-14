#!/bin/bash

PORT=1337

tshark -f"udp and port $PORT" -ilo -wudp.pcap &
sleep 1

(cd ../bin/echo; ./echo $PORT &)
sleep 1

(cd ../bin/nc; echo "deadbeef" |  ./nc 127.0.0.1 $PORT &)

killall nc
killall echo
killall tshark

wireshark udp.pcap &
