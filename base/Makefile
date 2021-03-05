CC = gcc
LD = gcc
CFLAGS = -g -Wall -Werror
LDFLAGS=
RM = /bin/rm -f
OBJS = gof.o
EXEC = gof

all: $(EXEC)

$(EXEC): $(OBJS)
	$(LD) -o $(EXEC) $(OBJS) $(LDFLAGS)

gof.o: gof.c gof.h
	$(CC) $(CFLAGS) -c gof.c

clean:
	$(RM) $(EXEC) $(OBJS)
