import tempfile
import os

import helper.proc


def test_pid_file():
    tfd, tpath = tempfile.mkstemp()
    os.close(tfd)
    out, pid = helper.proc.run(['-p', tpath])

    data = open(tpath, 'rt').read()
    assert pid == int(data)

