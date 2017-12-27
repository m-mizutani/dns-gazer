import msgpack
import io
import tempfile
import os
import helper.proc


def test_logging_transaction():
    out, pid = helper.proc.run(['-o', '-'])
    bs = io.BytesIO(out)

    # Get first message from iterator.
    msg_iter = msgpack.Unpacker(bs)
    msg1 = next(msg_iter)

    
    assert msg1[0] == b'dns-gazer.dns.tx'
    assert msg1[1] == 1444531212
    
    o1 = msg1[2]
    assert o1[b'client_addr'] == b'10.139.96.169'
    assert o1[b'server_addr'] == b'210.196.3.183'
    assert o1[b'server_port'] == 53
    
    assert len(o1[b'query']) == 1
    assert o1[b'query'][0][b'name'] == b'bf-pro-front.cloudapp.net.'
    assert o1[b'query'][0][b'type'] == b'A'
    assert o1[b'query'][0][b'section'] == b'question'

    assert len(o1[b'reply']) == 2
    assert o1[b'reply'][1][b'name'] == b'bf-pro-front.cloudapp.net.'
    assert o1[b'reply'][1][b'type'] == b'A'
    assert o1[b'reply'][1][b'data'] == b'23.100.102.231'
    assert o1[b'reply'][1][b'section'] == b'answer'

    
    msg2 = next(msg_iter)
    o2 = msg2[2]
    assert len(o2[b'query']) == 1
    assert o2[b'query'][0][b'name'] == b'news.nicovideo.jp.'
    assert o2[b'query'][0][b'type'] == b'A'
    assert o2[b'query'][0][b'section'] == b'question'

    assert len(o2[b'reply']) == 2
    assert o2[b'reply'][1][b'name'] == b'news.nicovideo.jp.'
    assert o2[b'reply'][1][b'type'] == b'A'
    assert o2[b'reply'][1][b'data'] == b'202.248.252.190'
    assert o2[b'reply'][1][b'section'] == b'answer'
    

def test_logging_record():
    # buffer size is not enough for subprocess.Popen.communicate()
    
    tfd, tpath = tempfile.mkstemp()
    os.close(tfd)
    out, pid = helper.proc.run(['-o', tpath, '-R'])

    # Get first message from iterator.
    it = msgpack.Unpacker(open(tpath, 'rb'))
    
    m1 = next(it)
    assert m1[0] == b'dns-gazer.dns.record'
    assert m1[2][b'msg_type'] == b'query'
    assert m1[2][b'name'] == b'bf-pro-front.cloudapp.net.'
    assert m1[2][b'section'] == b'question'
    assert m1[2][b'type'] == b'A'
    assert b'data' not in m1[2]

    m2 = next(it)
    assert m2[0] == b'dns-gazer.dns.tx'

    m3 = next(it)
    assert m3[0] == b'dns-gazer.dns.record'
    assert m3[2][b'msg_type'] == b'reply'
    assert m3[2][b'name'] == b'bf-pro-front.cloudapp.net.'
    assert m3[2][b'section'] == b'question'
    assert m3[2][b'type'] == b'A'
    assert b'data' not in m3[2]

    m4 = next(it)
    assert m4[0] == b'dns-gazer.dns.record'
    assert m4[2][b'msg_type'] == b'reply'
    assert m4[2][b'name'] == b'bf-pro-front.cloudapp.net.'
    assert m4[2][b'section'] == b'answer'
    assert m4[2][b'type'] == b'A'
    assert m4[2][b'data'] == b'23.100.102.231'
    
