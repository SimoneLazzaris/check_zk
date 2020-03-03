# CHECK_ZK

Simple nagios plugins that check if zookeeper responds correctly via TCP.

Written in C++ for speed and lightness. Needs cmake and boost.

## Build instructions:

To build on debian 10:
```bash
apt-get install cmake 
apt-get install libboost-thread1.67-dev libboost-system1.67-dev libboost-program-options1.67-dev libboost-regex1.67-dev
cmake .
make
```
