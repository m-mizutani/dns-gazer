import tempfile
import os
import signal
import psutil

import helper.proc


def test_pid_file():
    tfd, tpath = tempfile.mkstemp()
    os.close(tfd)
    out, pid = helper.proc.run(['-p', tpath])

    data = open(tpath, 'rt').read()
    assert pid == int(data)


def test_demonize():
    tfd, tpath = tempfile.mkstemp()
    os.close(tfd)
    out, pid = helper.proc.run(['-p', tpath, '-d'])

    demon_pid = int(open(tpath, 'rt').read())
    assert pid != demon_pid
