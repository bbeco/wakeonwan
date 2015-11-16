CFLAGS=-Wall -Werror -DDEBUG

.PHONY=clean

all: server

clean:
	rm -fr *.o server
