#!/bin/sh
case "$1" in
	start)
		/usr/bin/x11vnc -display :0 -forever -shared -bg -o /var/log/x11vnc.log &
		;;
	stop)
		echo "stop x11vnc finished"
		;;
	restart|reload)
		$0 stop
		$0 start
		;;
	*)
		echo "Usage: $0 {start|stop|restart}"
		exit 1
esac

exit 0
