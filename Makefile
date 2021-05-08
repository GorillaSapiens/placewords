CC=gcc
CFLAGS=-g -O
LIBS=-lm

placewords: placewords.o words.o s2.o main.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

.c.o:
	$(CC) -c $(CFLAGS) $<

