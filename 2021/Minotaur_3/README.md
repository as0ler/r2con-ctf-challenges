Keeping up with the Minotaur
----------------------------

The latest game patch claims to have finally made the game unbeatable by hackers. Can you catch up with modern times?

ssh://user@password:XXX.XXX.XXX.XXX

Files
-----

./game

Challenge Type
--------------

Reversing / Coding

Difficulty
----------

Medium

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

This is basically the first challenge, but now the mazes are actually random. Well, pseudo random, but based on the current minute in UTC time. Players have two choices: one is to accurately predict the pseudorandom and have a predefined solution. The other choice is to code a generic maze solving algorithm.

I think the generic solution is interesting because if any of the players at any point figures that one out, one solution solves all the challenges except for the glitched one! :D
