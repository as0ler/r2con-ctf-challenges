#!/usr/bin/python2

with open("shit-fs_patched", "rb") as fd:
    a = fd.read()

s = "\x48\x89\xC2\x48\x89\xDE\xE8"
x = "\x48\x31\xC0\x90\x90"
p = a.find(s)
assert p > len(s)
p = p + len(s) - 1
q = p + len(x)
b = a[:p] + x + a[q:]
assert len(a) == len(b)

with open("shit-fs_patched", "wb") as fd:
    fd.write(b)
