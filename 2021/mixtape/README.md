My Brother's Mixtape
--------------------

I found one of my older brother's mixtapes and converted it to mp3 but it sounds really weird... maybe 80's music is worse than I thought!

Files
-----

mixtape.mp3

Difficulty
----------

Medium / Hard

Solution
--------

The so called "mixtape" starts with a retro sounding voice saying "PRESS PLAY ON TAPE", followed by obviously encoded content. The phrase is a hint that this is a Commodore64 encoded tape backup. (Fun fact, I generated that retro voice using an actual Commodore64 text to speech generator).

The audio needs to be converted from .mp3 to .wav (for example using ffmpeg -i mixtape.mp3 mixtape.wav), then converted to a .tap file using Audiotap:

* https://wav-prg.sourceforge.io/

Then the .tap file needs to be loaded in an emulator. I used Vice:

* https://vice-emu.sourceforge.io/

Inside the tape there is the following PET BASIC program:

```
1 REM enable shifted mode
10 DATA 67,74,202,74,66
20 DATA 50,53,55,214,199
30 DATA 71,90,216,48,210
40 DATA 89,205,90,210,84
50 DATA 216,90,194,77,216
60 DATA 49,210,79,205,49
70 DATA 56,52,205,200,206
80 DATA 70,211,216,206,70
90 DATA 209,215,87,88,68
100 DATA 74,206,57
110 A$=""
120 FOR I=1 TO 48
130 READ B
140 A$=A$+CHR$(B)
150 NEXT I
160 PRINT A
```

The program doesn't work. First off, line 160 has a type error that causes the output of the program to be just the number 0. We fix it:

```
160 PRINT A$
```

Now we run the program but get some garbage output, so we need to enter PETSCII shifted mode.

```
PRINT CHR$(14)
```

Then we see the following output:

```
cjJjb257VGgzX0RyMzRtXzBmX1RoM184MHNfSXNfQWwxdjN9
```

This is base64 but it cannot be decoded inside the C64 emulator, so we copy it outside and decode it with "base64 -d":

```
r2con{Th3_Dr34m_0f_Th3_80s_Is_Al1v3}
```

Written like this the solution looks pretty straightforward, but it requires quite a lot of knowledge of C64 (which can be googled of course) and some tools to convert the audio to a .tap file to be loaded into an emulator. It can be quite frustrating for the younger players so I think it should be rewarded with wither Medium or High difficulty points (depending on how lenient y'all feel) since they'll have to spend some time researching how to decode and load tapes into an emulator, what PETSCII shifted mode even is, fixing a typo in a PET BASIC program, etc.

The only downside of this challenge is I never worked out a way to actually having to use r2 anywhere :( so I'm not sure if it's very useful.
