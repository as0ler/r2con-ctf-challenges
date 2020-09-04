import r2pipe
import sys
import itertools

passwords = map(''.join, itertools.product('abcdefghijklmnopqrstuvwxyz', repeat=1))

r2 = r2pipe.open("forceme")
r2.cmd("aeim")
for password in passwords:
	c = ord(password)
	sys.stdout.write("\rTesting %x   "%(c))
	sys.stdout.flush()
	r2.cmd("ar rax = 0x%x "%(c))
	r2.cmd("aepc sym._code")
	r2.cmd("aesu 0x100000913")
	res = r2.cmd("ar rax")
	print(res)
