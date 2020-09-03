import r2pipe
import json
import os

for filename in os.listdir('./binaries'):
    r = r2pipe.open(filename, flags=['-w'])
    print("[*] Patching binary syscalls")
    r.cmd('/x 48c7c0220000')
    r.cmd('wx 909090909090909090 @@ hit*')
