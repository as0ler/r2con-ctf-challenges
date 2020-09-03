import r2pipe
import json
import os

flag = ""

for i in range(0, 111):
    filename = 'binary{}'.format(i)
    file_path = os.path.join( './binaries' , filename)
    r = r2pipe.open(file_path)
    print("[*] Analyzing binary {}".format(file_path))
    print("[*] Finding operation")
    opcode = json.loads(r.cmd('pdj 1 @ 0x100000ba'))[0]['disasm']
    comparison = json.loads(r.cmd('pdj 1 @ 0x100000d4'))[0]['disasm']
    operator = opcode.split(',')[0].split()[0]
    print(operator)
    print("[*] Finding key")
    key = opcode.split(',')[1]
    print(key)
    print("[*] Finding secret")
    secret = comparison.split(',')[1]
    print(secret)

    if (operator == 'sub'):
        secret_char = chr(int(secret, 16) + int(key, 16) & 0xff)
    elif (operator == 'add'):
        secret_char = chr(int(secret, 16) - int(key, 16) & 0xff)
    elif (operator == 'xor'):
        secret_char = chr(int(key, 16) ^ int(secret, 16))

    print("Secret char found: {}".format(secret_char))
    flag += str(secret_char)

print("The flag is: {}".format(flag))
r.quit()
