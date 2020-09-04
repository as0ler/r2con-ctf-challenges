#!/usr/bin/env python2

import sys
import subprocess

def patch_exe():
    with open("radarelicensechecker.exe", "rb") as fd:
        a = fd.read()
    s = "\xB9\x01\x00\x00\x00\x44\x89\x64\x24\x24\x48\x89\xEF\xE8"
    p = a.find(s)
    assert p > len(s)
    p = p + len(s) - 1
    q = p + 5
    b = a[:p] + ('\x90' * 5) + a[q:]
    assert len(a) == len(b)
    with open("radarelicensechecker_patched.exe", "wb") as fd:
        fd.write(b)

def call_exe(attempt):
    p = subprocess.Popen(args=['wine', './radarelicensechecker_patched.exe', attempt], stdout=subprocess.PIPE)
    return p.communicate()[0]

def guess_flag_length():
    flag_len = 0
    for n in xrange(128):
        attempt = 'A' * n
        result = call_exe(attempt)
        if "wrong size of license string" not in result:
            flag_len = n
            break
    assert(flag_len != 0)
    return flag_len

def guess_flag(flag_len, echo = False):
    flag = ""
    characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ{}[]-_=+@:;<>,./?~#@\'"`\\|!$%^&*()'
    while len(flag) < flag_len:
        for char in characters:
            attempt = flag + char * (flag_len - len(flag))
            if echo:
                sys.stdout.write("\033[s" + attempt + "\033[u")
            result = call_exe(attempt)
            if "decryption error in block " not in result:
                flag = attempt
                break
            p = result.find("decryption error in block ") + len("decryption error in block ")
            q = result.find("!", p)
            block = int(result[p:q])
            if block > len(flag):
                flag = attempt[:block]
                break
    return attempt

def main():
    patch_exe()
    flag_len = guess_flag_length()
    print "Detected flag length: %d" % flag_len
    print "Bruteforcing flag..."
    flag = guess_flag(flag_len, echo = True)
    print

if __name__ == "__main__":
    main()
