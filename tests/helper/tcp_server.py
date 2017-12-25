#!/usr/bin/env python

import subprocess
import socket
import sys
import time


class Server:
    def __init__(self, port):
        self._port = int(port)

    def run(self):
        host = '127.0.0.1'
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind((host, self._port))
        sock.listen(1)

        client, client_addr = sock.accept()

        while True:
            recv_data = client.recv(4096)
            sys.stdout.buffer.write(recv_data)
            sys.stdout.buffer.flush()


class Proc:
    def __init__(self, port):
        self._port = port
        self._proc = subprocess.Popen([__file__, str(port)],
                                      stdout=subprocess.PIPE,
                                      stderr=subprocess.PIPE)
        time.sleep(0.5)    # delay
        
    def kill(self):
        time.sleep(0.5)    # delay
        self._proc.kill()
        out, err = self._proc.communicate()

        return out


if __name__ == '__main__':
    Server(sys.argv[1]).run()

    
