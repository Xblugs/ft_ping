#!/usr/bin/bash

# -v = verbose, -XX = formatting, -i any = from any card, icmp = protocol
sudo tcpdump -v -XX -i any icmp
