CFLAGS= -Wall -o3
SRCDIR=./src
BINDIR=./bin
CC=mpicc

all: $(BINDIR) $(BINDIR)/main

clean:
	rm -rf ./$(BINDIR)/main ./src/*.o

%.in:
	mpirun --machinefile ./inputs/hosts.txt --mca btl_tcp_if_include 10.20.221.0/24 ./bin/main ./inputs/$@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/main: $(SRCDIR)/knn.o $(SRCDIR)/parser.o
	$(CC) $(CFLAGS) $? $(SRCDIR)/main.c -o $(BINDIR)/main