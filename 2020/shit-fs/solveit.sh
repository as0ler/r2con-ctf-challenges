#!/bin/bash
set -e
set -x
cp shit-fs shit-fs_patched
upx -d shit-fs_patched
./patchit.py
chmod +x shit-fs_patched
mkdir tmp
./shit-fs_patched tmp flag.sfs fakepassword &
sleep 1
cat tmp/flag.txt
sudo umount tmp
rmdir tmp
