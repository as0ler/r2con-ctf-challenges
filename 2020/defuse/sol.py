import r2pipe
import sys
import itertools
import string

passwords = map(''.join, itertools.product('abcdefghijklmnopqrstuvwxyz', repeat=1))

r2 = r2pipe.open("defuse")
r2.cmd("aei")
for i in passwords:
  c = ord(i)
  print(i)
  # sys.stdout.write("\rTesting %x   "%(c))
  # sys.stdout.flush()
  print("---")
  print("Input RAX")
  r2.cmd("ar eax = 0x%x "%(int(c)))
  print(r2.cmd("ar eax"))
  r2.cmd("ar ebx = 0x%x "%(96))
  r2.cmd("s 0x000011d3")
  r2.cmd("aeip")
  r2.cmd("aesu 0x000011e8")
  print("Computed RAX")
  print(r2.cmd("ar eax"))
  print(r2.cmd("ar ebx"))
  print("---")
  if (r2.cmd("aer eax") == r2.cmd("aer ebx")): 
    print("Candidate to be a key: " + i)
    break
r2.quit()
