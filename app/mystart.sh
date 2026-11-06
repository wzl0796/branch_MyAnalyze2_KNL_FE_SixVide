#!/bin/sh

# set eth1
ifconfig eth1 down
ifconfig eth1 192.168.0.100 netmask 255.255.255.0 up

# if have network config file, set eth0
NETCONF=/ssd/cnf/cnf.network
if [ -s $NETCONF ] ; then
	source $NETCONF

	DEV_ADDR=`cat /sys/class/net/eth0/address`
	if [ "$DEV_ADDR" != "$MAC" ]; then
		ifconfig eth0 down
		ifconfig eth0 hw ether $MAC
	fi

	ifconfig eth0 $IP netmask $Mask up
	route add default gw $Gateway

	if ! grep -q "$DNS" /etc/resolv.conf; then
		echo nameserver 8.8.8.8 > /etc/resolv.conf
	fi
else
	ifconfig eth0 down
	ifconfig eth0 192.168.1.20 netmask 255.255.255.0 up
	route add default gw 192.168.1.1
	echo nameserver 192.168.1.1 > /etc/resolv.conf
fi

if [ ! -s /etc/ts.conf ]; then
      source /usr/bin/libinput_calibration.sh
      sync
fi

echo "209715200" > /proc/sys/net/ipv4/udp_rmem_min
echo "209715200" > /proc/sys/net/core/rmem_max
echo "209715200" > /proc/sys/net/core/rmem_default

# set remote network
arp -s 192.168.0.1 00:0a:35:01:fe:01
arp -s 192.168.0.2 00:0a:35:01:fe:02
arp -s 192.168.0.3 00:0a:35:01:fe:03
arp -s 192.168.0.4 00:0a:35:01:fe:04
arp -s 192.168.0.5 00:0a:35:01:fe:05
arp -s 192.168.0.6 00:0a:35:01:fe:06
arp -s 192.168.0.7 00:0a:35:01:fe:07
arp -s 192.168.0.8 00:0a:35:01:fe:08
arp -s 192.168.0.9 00:0a:35:01:fe:09
arp -s 192.168.0.10 00:0a:35:01:fe:0a

DIR=$PWD
export LD_LIBRARY_PATH=$DIR/lib/:$LD_LIBRARY_PATH

#移动web的库
APP_LIB=/app/plugs/libs
SYS_LIB=/userdata/qt5/lib

diff $APP_LIB/libQt5Network.so.5.9.9 $SYS_LIB/libQt5Network.so.5.9.9
if [ $? -ne 0 ]; then
    cp -f $APP_LIB/libQt5Network.so.5.9.9 $SYS_LIB/libQt5Network.so.5.9.9
fi

diff $APP_LIB/libQt5NetworkAuth.so.5.9.9 $SYS_LIB/libQt5NetworkAuth.so.5.9.9
if [ $? -ne 0 ]; then
    cp -f $APP_LIB/libQt5NetworkAuth.so.5.9.9 $SYS_LIB/libQt5NetworkAuth.so.5.9.9
fi

LIBCRYPTO="$SYS_LIB/libcrypto.a"
if [ ! -e "$LIBCRYPTO" ]; then
    cp -d $APP_LIB/libcrypto.* $SYS_LIB
fi

# replace S72x11vnc.sh
diff ./lib/S72x11vnc.sh /etc/init.d/S72x11vnc.sh
if [ $? -ne 0 ]; then
	echo "replace S72x11vnc.sh"
	cp -f ./lib/S72x11vnc.sh /etc/init.d/S72x11vnc.sh
	cd /etc/init.d/
	pwd
	sync
	cd -
	pwd
fi

LIBSSL="$SYS_LIB/libssl.a"
if [ ! -e "$LIBSSL" ]; then
    cp -d $APP_LIB/libssl* $SYS_LIB
fi

#ulimit -c unlimited
#echo "/userdata/core.%e.%p.%t" > /proc/sys/kernel/core_pattern

# start adjust app
./plugins/MyAdjust -platform xcb -geomtry 1920*1080 &

# start app
./MyAnalyze2_KA1 -platform xcb -geomtry 1920*1080
