#! /bin/sh

#./plugs/MyRemoteControl -platform linuxfb -geomtry 1024*768 -display VNC:LinuxFB &

killall MyRemoteControl
./plugs/MyRemoteControl -platform xcb -geomtry 1024*768
