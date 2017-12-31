import msgpack
import io
import tempfile
import os
import json

import helper.proc


def test_output_to_stdout():
    out, pid = helper.proc.run(['-o', '-'])
    bs = io.BytesIO(out)

    # Get first message from iterator.
    msg = next(msgpack.Unpacker(bs))

    assert msg[0] == b'dns-gazer.dns.tx'
    assert msg[1] == 1444531212
    assert msg[2][b'client_addr'] == b'10.139.96.169'


def test_output_to_file():
    tfd, tpath = tempfile.mkstemp()
    os.close(tfd)

    out, pid = helper.proc.run(['-o', tpath])
    msg = next(msgpack.Unpacker(open(tpath, 'rb')))

    assert out == b''
    assert msg[0] == b'dns-gazer.dns.tx'
    assert msg[1] == 1444531212
    assert msg[2][b'client_addr'] == b'10.139.96.169'
    os.unlink(tpath)


def test_output_to_textfile():
    tfd, tpath = tempfile.mkstemp()
    os.close(tfd)

    out, pid = helper.proc.run(['-t', tpath])
    assert out == b''
    
    lines = open(tpath, 'rt').read().split('\n')
    assert len(lines) > 100
    r1 = lines[0].split('\t')

    assert r1[0] == '2015-10-11T02:40:12+00:00'
    assert r1[1] == 'dns-gazer.dns.tx'
    jdata = json.loads(r1[2])
    assert jdata['client_addr'] == '10.139.96.169'
    assert isinstance(jdata['query'], list)
    assert isinstance(jdata['reply'], list)
    os.unlink(tpath)
