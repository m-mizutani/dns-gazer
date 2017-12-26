import msgpack
import io
import tempfile

import helper.proc


def test_output_to_stdout():
    out = helper.proc.run(['-o', '-'])
    bs = io.BytesIO(out)

    # Get first message from iterator.
    msg = next(msgpack.Unpacker(bs))
    print(msg[0])
    assert msg[0] == b'dns-gazer.dns.tx'
    assert msg[1] == 1444531212
    assert msg[2][b'client_addr'] == b'10.139.96.169'
