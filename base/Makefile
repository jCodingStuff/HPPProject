CC = gcc
LD = gcc
CFLAGS = -g -Wall -Werror
LDFLAGS=
RM = /bin/rm -f
OBJS = gol.o
EXEC = gol

all: $(EXEC)

$(EXEC): $(OBJS)
	$(LD) -o $(EXEC) $(OBJS) $(LDFLAGS)

gof.o: gol.c gol.h
	$(CC) $(CFLAGS) -c gol.c

clean:
	$(RM) $(EXEC) $(OBJS)
