CC = gcc
LD = gcc
CFLAGS = -g -O3 -Wall -Winline -march=native -ffast-math
LDFLAGS= -lpthread -ffast-math
RM = /bin/rm -f
OBJS = gol.o utils.o
EXEC = gol

all: $(EXEC)

$(EXEC): $(OBJS)
	$(LD) -o $(EXEC) $(OBJS) $(LDFLAGS)

gol.o: gol.c gol.h utils.h
	$(CC) $(CFLAGS) -c gol.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	$(RM) $(EXEC) $(OBJS)
