#!/bin/bash

port=`cat conf/proxy_port`

ssh localhost -ND 127.0.0.1:$port

