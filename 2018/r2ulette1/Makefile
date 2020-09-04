all:
	gcc -static  -fno-stack-protector -z execstack src/r2ulette.c -o bin/r2ulette
	gcc -static  -fno-stack-protector -z execstack src/r2ulette.c -o bin/r2ulette_s
	strip bin/r2ulette_s

clean:
	rm -f bin/r2ulette
