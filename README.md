DNS Gazer
============

DNS Gazer is a passive DNS packet analysis and logging tool. From security operation view, DNS query and reply logs are important to detect and investigate a security incidnet. An attacker who compromised the system use an unique and strange domain name. Tehse domain names are often shared as IOC (Indicator Of Compromise). Then,

- A security operator can detect malware infection, trojan activity and so on by comparing domain names that are appeared in internal DNS query/reply logs and shared ones.
- Some malware uses DNS query to communicate with C2 (Command & Control) server. A security operator can find out which host communicates with C2 server in incident response.

To gathering DNS query and reply logs, DNS Gazer has following advantages.

- Not only storing log to local file, but also continuously sending logs to [fluentd](https://www.fluentd.org/). 
- Support both of a "transaction log" including query + reply and a "record log" that is extracted from DNS packet as indivisual record of each DNS section (Qeustion, Answer, etc.)

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
  -o STRING             Output to a file as msgpack foramt, '-' is stdout
  -t STRING             Output to a file as text foramt, '-' is stdout
  -p STRING             pid file path
  -d                    Enable demon mode
  -R                    Enable logging per record
  -v                    Show version
```

Getting started
----------

### Use case 1: Sending DNS transaction logs from network interface `eth0` to fluentd on `localhost` and port `24224`


```shell
$ dns-gazer -i eth0 -f localhost:24224
```

Then, fluentd receives following logs.

```
2017-12-21 17:58:00.000000000 +0900 dns-gazer.dns.tx: {
  "client_addr": "10.139.96.169",
  "client_port": 53684,
  "query": [
    {
      "name": "bf-pro-front.cloudapp.net.",
      "section": "question",
      "type": "A"
    }
  ],
  "query_ts": 1444531212.628222,
  "reply": [
    {
      "name": "bf-pro-front.cloudapp.net.",
      "section": "question",
      "type": "A"
    },
    {
      "data": "23.100.102.231",
      "name": "bf-pro-front.cloudapp.net.",
      "section": "answer",
      "type": "A"
    }
  ],
  "reply_ts": 1444531212.643494,
  "server_addr": "210.196.3.183",
  "server_port": 53,
  "status": "success",
  "tx_id": 23904
}
```

(Pretty printed for readability)

### Use case 2: Printing DNS transaction logs from `eth0` to stdout

```shell
$ dns-gazer -i eth0 -t -
```

Then, output following logs to stdout.

```
2015-10-11T02:40:12+00:00	dns-gazer.dns.tx	{"client_addr": "10.139.96.169", "client_port": 53684, "query": [{"name": "bf-pro-front.cloudapp.net.", "section": "question", "type": "A"}], "query_ts": 1.44453e+09, "reply": [{"name": "bf-pro-front.cloudapp.net.", "section": "question", "type": "A"}, {"data": "23.100.102.231", "name": "bf-pro-front.cloudapp.net.", "section": "answer", "type": "A"}], "reply_ts": 1.44453e+09, "server_addr": "210.196.3.183", "server_port": 53, "status": "success", "tx_id": 23904}
```

### Use case 3: Printing not only DNS transaction logs but also "record logs" from `eth0` to stdout

"Record log" means logs per DNS record. dns-gazer splits a DNS query/reply to multiple records by `-R` optoin. The record logs are for log managemennt system based on non-nested dictionary data type, such as Graylog. The record logs don't have nested dictionary and array.

```shell
$ dns-gazer -r captured.pcap -R -t -
```

Then, output flowwing logs to stdout.

```
2015-10-11T02:40:12+00:00	dns-gazer.dns.record	{"client_addr": "10.139.96.169", "client_port": 53684, "msg_type": "query", "name": "bf-pro-front.cloudapp.net.", "section": "question", "server_addr": "210.196.3.183", "server_port": 53, "tx_id": 23904, "type": "A"}
2015-10-11T02:40:12+00:00	dns-gazer.dns.tx	{"client_addr": "10.139.96.169", "client_port": 53684, "query": [{"name": "bf-pro-front.cloudapp.net.", "section": "question", "type": "A"}], "query_ts": 1.44453e+09, "reply": [{"name": "bf-pro-front.cloudapp.net.", "section": "question", "type": "A"}, {"data": "23.100.102.231", "name": "bf-pro-front.cloudapp.net.", "section": "answer", "type": "A"}], "reply_ts": 1.44453e+09, "server_addr": "210.196.3.183", "server_port": 53, "status": "success", "tx_id": 23904}
2015-10-11T02:40:12+00:00	dns-gazer.dns.record	{"client_addr": "10.139.96.169", "client_port": 53684, "msg_type": "reply", "name": "bf-pro-front.cloudapp.net.", "section": "question", "server_addr": "210.196.3.183", "server_port": 53, "tx_id": 23904, "type": "A"}
2015-10-11T02:40:12+00:00	dns-gazer.dns.record	{"client_addr": "10.139.96.169", "client_port": 53684, "data": "23.100.102.231", "msg_type": "reply", "name": "bf-pro-front.cloudapp.net.", "section": "answer", "server_addr": "210.196.3.183", "server_port": 53, "tx_id": 23904, "type": "A"}

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

### Build & install

```shell
$ git clone --recurse-submodules git@github.com:m-mizutani/dns-gazer.git
$ cd dns-gazer
$ cmake .
$ make
$ sudo make install
```

If you have extra third party package directory (such as HomeBrew) and required libraries are in there, you can specify `INC_DIR` and `LIB_DIR` for `cmake`.

```shell
$ cmake -DINC_DIR=/opt/include -DLIB_DIR=/opt/lib .
```




