CC=gcc
CFLAGS=-Wall -g

all: make-2d print-2d mm blocked-mm mydiff

make-2d: make-2d.c
	$(CC) $(CFLAGS) -o make-2d make-2d.c
print-2d: print-2d.c
	$(CC) $(CFLAGS) -o print-2d print-2d.c
mm: mm.c
	$(CC) $(CFLAGS) -o mm mm.c
blocked-mm:  blocked-mm.c
	$(CC) $(CFLAGS) -o blocked-mm blocked-mm.c
mydiff: mydiff.c
	$(CC) $(CFLAGS) -o mydiff mydiff.c
clean:
	rm -f *.o make-2d print-2d mm blocked-mm mydiff
	
