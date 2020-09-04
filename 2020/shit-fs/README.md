Challenge description
---------------------

The newest military grade, close source, encrypted filesystem out there is called Secure Hidden Invulnerable Transactional File System, or shit-fs for short. Runs on Ubuntu 18.04 and requires FUSE and libsodium. The vendor claims their crypto is unbreakable but some say it's not as good as it's cracked up to be (pun intended). Can YOU crack the encrypted filesystem and get the flag?

Challenge files
---------------

* shit-fs
* flag.sfs

Compiling
---------

The following dependencies are required to compile and pack the binary:

* GCC
* FUSE + development headers
* Libsodium + development headers
* UPX for Linux

On a Debian based system that would be:

```
sudo apt-get install build-essential fuse libfuse-dev libsodium-dev upx-ucl
```

Once the dependencies are installed just invoke the makefile:

```
make clean && make
```

The makefile will read the flag value from flag.txt, compile the challenge binary, strip the debug symbols, pack it with UPX, then use the binary itself to create the flag.sfs file that will contain the encrypted flag.

Solution
--------

TL;DR: Check out solveit.sh

The joke that was in my head and probably no one else would find funny was trying to come up with a crypto challenge where I did everything exactly like the libsodium documentation said, and still managed to write something stupid and broken, proving crypto isn't magic pixie dust that makes everything secure. I didn't quite reach that point because I was lazy, but I came close.

Anyway.

The "encrypted" flag.sfs volume indeed does encrypt data using fancy secure algorithms, but stores the crypto keys in the file header. The password is hashed, and also stored in the flag.sfs header. The easiest solution consists of realizing this and patching the call to crypto_pwhash_str_verify() to bypass the "authentication". A harder way to solve it is to parse the file yourself, but I doubt anyone would try that.

It would have been perfect if each file stream had been encrypted flawlessly and without reusing nonces and all that stuff in a properly implemented filesystem that would automatically grow in size... but I didn't have the time to do that, so I just put a single key and a single nonce in the header, made all files 1k maximum, limited the number of files to 256, indexed the files like a big array and called it a day. Doesn't even matter since the bug is somewhere else anyway.
