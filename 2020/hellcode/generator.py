#!/usr/bin/env python

from pwn import *
from random import *
from shutil import copyfile

text = """Roses are red, violets are blue, the flag is r2con{0verF33ding_oF_Binari3s}, and all those binaries are for you"""

context(arch='amd64')

for i, c in enumerate(text):
    operator = ["xor", "sub", "add"][randint(0, 2)]
    key = randint(1, 100)

    if operator == "xor":
        secret = (ord(c) ^ key) & 0xff

    if operator == "sub":
        secret = (ord(c) - key) & 0xff

    if operator == "add":
        secret = (ord(c) + key) & 0xff

    init = """
    push rbp
    mov rbp, rsp
    """
    fini = """
    mov rsp, rbp
    pop rbp
    ret
    """
    pause = """
    mov rax, 0x22
    syscall
    """

    msg = shellcraft.echo("ah ah ah, you didn't say the magic word...\n")
    good_boy = shellcraft.echo("Good! You are one step closer.\n")
    bad_boy = shellcraft.echo("Bad choice!\n")
    read_to_dl = "{}\tmov rdx,[rsp]\n".format(shellcraft.amd64.linux.read())
    oper = "\t{} dl, {}".format(operator, key)
    exit_asm = """
    mov rdi, 0
    mov rax, 0x3c
    syscall
    """

    check = """
    cmp dl, {}
    jne error
    {} 
    {}
    error:
        {} 
        {}
    """.format(secret, good_boy, exit_asm, bad_boy, exit_asm)
       
    jmp1_to_trampoline = """
    trolololololololo:
    je trolo
    jne trolo
    trololololo:
    """

    
    jmp2_to_trampoline = """
    mov rbx, 1
    xor rax, rax
    je trolololololo
    trololo:
    xor rax, 1 
    cmp rax, rbx
    xor rbx, rbx
    jne trololo
    """

    trampoline = """
    trolo:
    push 0x65
    push 0x6d20776f
    push 0x6c6c6f46
    xor rax, rax
    je trololololo
    trolololololo:
    mov rax, 0x1010101
    xor rax, 0x1017c78
    push rax
    push 0x7361655f
    push 0x30735f74
    push 0x306e7b6e
    push 0x6f633272
    push 0x20736920
    push 0x67616c66
    push 0x20656854
    jmp trololo
    """

    not_a_jmp = """
    xor rcx, rcx
    jne trolololololololo 
    """

    code = init + pause + msg + pause + jmp1_to_trampoline + pause + read_to_dl + pause + jmp2_to_trampoline + pause + oper + pause + not_a_jmp + pause + check + pause + trampoline + fini

    print("[* ] Generating file {}".format(i))
    elf = make_elf_from_assembly(code)
    copyfile(elf, "binaries/binary{}".format(i))
