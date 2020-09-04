#!/bin/bash
set -x
mkdir tmp
rm -f flag.sfs
python -c "import random; import string; a = string.ascii_letters + string.digits; print ''.join(random.choice(a) for _ in xrange(32))" > password.txt
./shit-fs tmp flag.sfs $(cat password.txt) &
sleep 1
cp flag.txt tmp/
sync flag.sfs
sudo umount tmp
rmdir tmp
