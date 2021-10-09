Keeping up with the Minotaur
----------------------------

Alright, enough cheating! This one has to be solved in an honest way. :)

ssh://user@password:XXX.XXX.XXX.XXX

Files
-----

./game

Challenge Type
--------------

Reversing / Coding

Difficulty
----------

Hard

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

Final variant on this challenge. This one has no cheat, it really must be solved using an algorithm.

Players who solve this one first can basically throw the same solution at all of the previous challenges except for the glitched one.
