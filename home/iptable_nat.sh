#!/bin/sh
#tested interface named enp0s25
iptables -F
iptables -t nat -A POSTROUTING -s 192.168.191.0/24 -o enp0s25 -j SNAT --to 10.237.0.1
iptables -t nat -A PREROUTING -d 10.237.0.1 -i enp0s25 -p tcp --dport 8888 -j DNAT --to 192.168.191.10
iptables -P FORWARD ACCEPT
iptables -P OUTPUT ACCEPT
iptables -P INPUT ACCEPT
