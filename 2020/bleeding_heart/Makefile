all:
	~/build/bin/clang -v src/bleeding_heart.c -s -o dist/bleeding_heart -mllvm -sub -mllvm -bcf -mllvm -fla -mllvm -sobf 
	gcc -Wall -o bin/bleeding_heart src/bleeding_heart.c

clean:
	rm -f dist/bleeding_heart
	rm -f bin/bleeding_heart
