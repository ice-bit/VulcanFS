CC=gcc
CFLAGS=-Wall -Wextra -Werror

all:
	$(CC) $(CFLAGS) vulcanfs.c -o vulcanfs

clean:
	rm vulcanfs initrd.img