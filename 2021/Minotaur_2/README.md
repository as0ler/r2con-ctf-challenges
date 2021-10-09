Hunting the Minotaur, revisited
-------------------------------

Looks like the developers caught wind of what you were doing and patched the game. Can you beat it now?

ssh://user@password:XXX.XXX.XXX.XXX

Files
-----

./game

Challenge Type
--------------

Reversing / Coding

Difficulty
----------

Easy / Medium

Deployment
----------

To compile just do:

```
$ go build
```

Players should be given the precompiled binary, which is also set up on a Docker container accessible over SSH with username and password. The SSH daemon needs to be configured to disallow port forwarding and the shell must be replaced by the game binary so players don't actually get a shell (kinda like setting up a git repo).

Alternatively telnet could be used instead of SSH, but there must be a TTY or the game won't work (pipe and netcat won't do!).

Flag goes in flag.txt in current directory in the container.

WARNING: the SSH connection cannot be too slow, or the players won't be able to beat the game on time! Make sure to test the solution after deployment. If the solution doesn't work, patch the source code to increase the time limit and recompile.

Solution
--------

This challenge is identical to the previous one, except the WASD keys no longer work. I would expect players to use the exact same key sequences as before, but instead of copying and pasting, they'll have to use some macro generation tool or custom script with Paramiko to send arrow keystrokes instead of letters.

The solveit.py example script solves the game.
