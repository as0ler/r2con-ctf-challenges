CC=x86_64-w64-mingw32-gcc
CFLAGS=-I. -D DO_SLEEP -O2
DEPS = main.h rc4.h flag.h
OBJ = main.o rc4.o

radarelicensechecker.exe: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	strip radarelicensechecker.exe
	./solveit.py

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

flag.h: genflag.c genflag.py
	gcc -I. -g -c -o genflag.o genflag.c
	gcc -I. -g -o genflag genflag.o
	./genflag.py

clean:
	rm -f radarelicensechecker.exe radarelicensechecker_patched.exe flag.h genflag *.o
