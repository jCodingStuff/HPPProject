CC = gcc-10
LD = gcc-10
CFLAGS = -g -Wall -Werror
LDFLAGS=
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
