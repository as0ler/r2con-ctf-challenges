#!/usr/bin/env python





# INSERT FLAG HERE
flag = "r2con{D0nt_Do_Crypt0_At_Hom3_Kids}"


# CHOOSE THE TEXT HERE
#textfile = "hacker.txt"
textfile = "banner.txt"







with open(textfile, "rU") as fd:
    text = fd.read()

chunk_size = int( len(text) / (len(flag) - 1) )
chunks = [ text[ i * chunk_size : (i+1) * chunk_size ] for i in range(len(flag)) ]

rounded_size = int(64 * round(float(chunk_size+1)/64))
chunks = [ c + "\0" + ("\0" * (rounded_size - len(c))) for c in chunks ]

print
print "Flag is: %s" % flag
print "Text is: %s" % textfile
#print "Splitting text into %i encrypted chunks of %i bytes each" % (len(chunks), rounded_size)
print

assert len(flag) == len(chunks)

import subprocess
def encrypt(key, chunk):
    p = subprocess.Popen(args=['./genflag', key], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    encrypted, error = p.communicate(chunk)
    status = p.wait()
    if error:
        raise Exception(error)
    if status != 0:
        raise Exception(str(status))
    #print
    #print key
    #print repr(chunk)
    #print repr(encrypted)
    assert len(encrypted) == len(chunk)
    return encrypted

encrypted_chunks = []
i = 0
for chunk in chunks:
    encrypted_chunks.append( encrypt(flag[i], chunk) )
    i = i + 1

with open("flag.h", "w") as fd:
    i = 0
    #for chunk in chunks:
    for chunk in encrypted_chunks:
        #print chunk,
        #print "Chunk %i, %i bytes" % (i, len(chunk))
        chunk = [ hex(ord(c)) for c in chunk ]
        fd.write("static unsigned char chunk_%i[] = {" % i)
        fd.write(", ".join(chunk))
        fd.write("};\n")
        i = i + 1
    chunk_names = [ "chunk_%i" % i for i in range(len(chunks)) ]
    fd.write("static unsigned char *chunks[] = {")
    fd.write(", ".join(chunk_names))
    fd.write("};\n")
