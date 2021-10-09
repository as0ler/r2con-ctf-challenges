Cheating Minotaur
-----------------

A secret bonus level was discovered! But it looks like the developers made a mistake and it's impossible to beat...

ssh://user@password:XXX.XXX.XXX.XXX

Files
-----

./game

Challenge Type
--------------

Reversing / Misc

Difficulty
----------

Easy

Deployment
----------

To compile just do:

```
$ go build
```

Players should be given the precompiled binary, which is also set up on a Docker container accessible over SSH with username and password. The SSH daemon needs to be configured to disallow port forwarding and the shell must be replaced by the game binary so players don't actually get a shell (kinda like setting up a git repo).

Alternatively telnet could be used instead of SSH, but there must be a TTY or the game won't work (pipe and netcat won't do!).

Flag goes in flag.txt in current directory in the container.

Solution
--------

Players should reverse engineer the game to figure out how the game detects collisions - namely, by reading the screen and seeing if there is a wall. By resizing the console, the game can be forced not to draw a wall, and you can escape.
