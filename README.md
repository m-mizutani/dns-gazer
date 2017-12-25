DNS Gazer
============

Passive DNS packet analysis and logging tool.

Usage
--------

```
$ dns-gazer  --help
Usage: dns-gazer [options]

Options:
  -h, --help            show this help message and exit
  -r STRING             read pcap format file
  -i STRING             capture interface
  -f STRING             Fluentd destination, e.g. 127.0.0.1:24224
  -o STRING             Log file path, '-' is stdout
  -v                    Show version
```

Quick start
----------

Assume following

- You have fluentd process on `localhost` and port `24224`
- You want to monitor on network interface `eth0`

Then,

```shell
$ dns-gazer -i eth0 -f localhost:24224
```

Setup
----------


### Prerequisite

- Support OS: Linux and macOSX
    - Linux >= 4.4.0
    - macOSX >= 10.12.6
- C++11 compiler (Recommend: clang++ >= 3.9)
- CMake >= 3.5.1
- libpcap >= 1.7.4
- libmsgpack >= 0.5.9

### build

```shell
$ git clone --recurse-submodules git@ghe.ckpd.co:mizutani/dns-gazer.git
$ cd dns-gazer
$ cmake .
$ make
$ sudo make install
```

If you have extra third party package directory (such as HomeBrew) and required libraries are in there, you can specify `INC_DIR` and `LIB_DIR` for `cmake`.

```shell
$ cmake -DINC_DIR=/opt/include -DLIB_DIR=/opt/lib .
```




