CC=gcc
CFLAGS=-I. -O2 -lsodium `pkg-config fuse --cflags --libs`
#CFLAGS=-I. -g -DDEBUG -DNOCRYPTO -lsodium `pkg-config fuse --cflags --libs`
DEPS = shit-fs.c
OBJ = shit-fs.o

flag.sfs: shit-fs genflag.sh
	./genflag.sh
	./solveit.sh

shit-fs: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	strip shit-fs
	upx -9 shit-fs

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f shit-fs password.txt flag.sfs *.o
	rm -fr tmp/
