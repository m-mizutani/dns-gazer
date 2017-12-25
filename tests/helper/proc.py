import subprocess


def run(args):
    base_args = ['./bin/dns-gazer', '-r', './tests/sample.pcap']
    proc = subprocess.Popen(base_args + args, stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    
    try:
        proc.wait(timeout=15)
    except subprocess.TimeoutExpired:
        proc.kill()

    outs, errs = proc.communicate()

    return outs
