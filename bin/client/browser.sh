#!/bin/bash

URL=http://www.google.se

port=`cat conf/port`

curl --proxy socks5h://localhost:$port $URL

