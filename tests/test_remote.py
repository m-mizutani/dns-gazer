import msgpack
import io

import helper.proc
import helper.tcp_server


def test_output_to_fluentd():
    
    port = 34334
    server = helper.tcp_server.Proc(port)
    out = helper.proc.run(['-f', 'localhost:{}'.format(port)])
    out = server.kill()

    # Get first message from iterator.
    msg = next(msgpack.Unpacker(io.BytesIO(out)))
    
    assert msg[0] == b'dns-gazer.dns.tx'
    assert msg[1] == 1444531212
    assert msg[2][b'client_addr'] == b'10.139.96.169'
