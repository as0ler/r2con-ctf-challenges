all:
	~/obf/bin/clang -v src/cerberus.c -s -o dist/cerberus -mllvm -sub -mllvm -fla -mllvm -sobf 
	gcc -Wall -o bin/cerberus src/cerberus.c

clean:
	rm -f dist/cerberus
	rm -f bin/cerberus
