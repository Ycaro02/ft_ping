#!/bin/bash

ip a | grep wlan | grep inet | awk '{print $2}' | cut -d '/' -f 1 > .myip.txt
