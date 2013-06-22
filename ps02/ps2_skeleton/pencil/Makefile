all: clean scanner
	cc -std=gnu99 -o pencil pencil.c scanner.c -lm
scanner:
	flex -oscanner.c scanner.l
clean:
	rm -f pencil scanner.c
