CC=gcc
CFLAGS=-g -O
LIBS=-lm

placewords: main.o placewords.o words.o s2.o
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $<

