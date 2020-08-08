CC	= /opt/gbdk/bin/lcc -Wa-l -Wl-m -DGBDK_2_COMPAT=1

BINS	= r2tune.gb

all:	$(BINS)

%.s:	%.c
	$(CC) -S -o $@ $<
	
%.o:	%.c
	$(CC) -c -o $@ $<

%.o:	%.s
	$(CC) -c -o $@ $<

%.gb:	%.o
	$(CC) -o $@ $<

clean:
	rm -f *.o *.lst *.map *.gb
