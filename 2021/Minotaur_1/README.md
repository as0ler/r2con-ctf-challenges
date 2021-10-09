Hunting the Minotaur
--------------------

Can you break a new speed record in this exciting game? Connect through ssh://user@password:XXX.XXX.XXX.XXX and find out!

Files
-----

./game

Challenge Type
--------------

Reversing / Coding

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

WARNING: the SSH connection cannot be too slow, or the players won't be able to beat the game on time! Make sure to test the solution after deployment. If the solution doesn't work, patch the source code to increase the time limit and recompile.

Solution
--------

Players should reverse engineer the game to figure out they need to beat the game in under 1:30 minutes to get the flag, which is impossible to do for a human. I would expect players would solve the maze manually and take note of all the keys needed to press to win, then copy and paste (since the game uses WASD controls). Some may beat it faster by using some kind of macro recording tool.

The actual key sequences are:

```
dsdwdsssdsdwdssassdd
ddsdwdsdwdsdssassasdds
ssssssdsdddddd
dsasddddwwdsdwdssssawwassawassawaasssdddwdddsd
ddssaasssssddddddd
dddsdwdsssssdwdssasd
ssdsdsaasssdwwddssdddd
sssssssddddwddwdss
ddddsdsdwwdsssssasds
dddssdsdwwawddsdsssasssd
dsasssddssddsddwwdss
dsdwdddddssssssasd
sssddwdsddssssdwwwwdssss
dddddsaaasassdwddsdwdssdss
dddssddwwddsasdsssss
ssssdsasdsdddddd
ssssssdddsdddwawwdsdss
dsasdssasdddsddwwawawdwddwdsssssasds
ddsasdssdwwdwawddsddsssasdsasd
dddsdwdddsasdssassds
ssddssssasdddddd
sssssddwdsdwdssdwdss
sdwdsdsasawassssdddwdsdwwaawdwdwdssdssss
ssssdsassdwdsddwdsdwds
ddsssawasssdwdsdssddwdsd
ddsddsdsaawaasssasdddddwdssd
dsasssdsssddwwdsdsdd
dsasdddsasdssasddwdsdd
sdwdsdwdssddwawddsssssawasssdwds
dsddwddsddsasssdsaaaasdddd
sssssdsddwwdwwdsssasdsdd
sdwddsasasssssdwwddssdwdsd
sssssssdddwwwdsdsdwwawwwawdddssasdssss
sssddwdssdsassdddd
ssssssdwwdsdsasddwwdwawdwddsassdsaasdd
sssddwwawddddsaasdsdsdwwwwdsssssss
sddwddsasssdsssddd
ssdssassddsasddwdwawdwdwwdwdssasdsasassdwds
sssdwwdssdwdwdddssssss
ddsaasdddwwddddssssssasd
sdsdssaassddsddddwds
sssssdddssdddd
```

