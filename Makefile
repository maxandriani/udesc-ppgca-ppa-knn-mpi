CFLAGS= -Wall -o3 -pg
SRCDIR=./src
BINDIR=./bin
CC=gcc

all: $(BINDIR) $(BINDIR)/main

clean:
	rm -rf ./$(BINDIR)/main ./src/*.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/main: $(SRCDIR)/knn.o $(SRCDIR)/parser.o
	$(CC) $(CFLAGS) $? $(SRCDIR)/main.c -o $(BINDIR)/main