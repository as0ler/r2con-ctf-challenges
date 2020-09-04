Challenge description
---------------------

A Radare Corporation employee has leaked the internal license checking tool for the new version of Radare Pro for Windows. Only YOU can you crack the licensing scheme and make sure Radare remains free!

Challenge files
---------------

* radarelicensechecker.exe

Compiling
---------

TL;DR: The provided makefile will cross-compile from Linux using Mingw. Put the flag in the genflag.py file and just run make. The default flag is "flag{DontDoCryptoAtHomeKids}" so if you don't need to change it just distribute the binary to the players and you're done. :)

Long version: the challenge was originally developed on Linux, then cross-compiled. By uncommenting some lines in the makefile you can choose whether to create a Linux or Windows binary. You can also enable or disable the sleep() call (more on that later) and control whether to include debugging symbols or not. This allows you to tweak the difficulty of the challenge as you see fit.

In its most easy form, you can just give the players the actual source code to main.c, rc4.c and flag.h so they solve it as a pure crypto challenge. In its hardest form, players are given just a Windows binary without symbols and with optimizations turned on, so they have to reverse the binary first to understand what's going on, then figure out what the crypto problem is, then patch the binary to speed it up so the crypto can be bruteforced, then brutforce the flag. You can choose any option in between to make it exactly as difficult as you want.

Additionally you can customize the encrypted text inside the binary by editing genflag.py as well. As an example I provided you with two options: hacker.txt (the Hacker Manifesto) and banner.txt (an ASCII art banner).

Solution
--------

TL;DR: Check out solveit.py for a possible solution.

The binary takes the flag as its "license", via command line. Each byte of the flag is used to seed an RC4 key, which is used to decrypt a chunk of text. If one of the chunks doesn't decrypt properly, an error is shown at that exact moment. The player has to figure out that since each char is a key to a different block, you can use this as an oracle and brute force the flag one character at a time, with 256 possible keys for each chunk (well, a lot less really, since they're printable characters).

As I mentioned before, you can choose how hard to make this to players, by either giving them some of the sources, or enabling debugging symbols so they figure out quicker it's RC4.

It's a Windows binary just to mess with players a little, but you don't really need Windows for this. If you're an absolute pro you can reverse the binary, figure out what it's doing and solve it without running the tool at all - but it's a hell of a lot faster to solve if you just write a script and run it on Wine. Again, you can choose if you want to cross compile to Windows or just stick with a Linux binary.

There is one additional hurdle, the binary makes a lot of annoying sleep() calls just to mess with the bruteforce attempts, because I'm evil like that. Players will have to patch the binary to bypass the sleep() calls. This can be disabled in the makefile if you don't like it.
