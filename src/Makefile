CC = gcc
SOURCES = $(wildcard *.c)
TARGET = $(SOURCES:%.c=%)
CFLAGS = -g -Wall
all: $(TARGET)
%.o: %.c
%: %.c
	$(CC) $(CFLAGS) -o $@ $<
run_%: %
	./$< $(TESTARGS) 
deamon:
	$(CC) $(CFLAGS) -o pokemond pokemond.c
	./pokemond
clean:
	@rm -rf *.o $(TARGET)