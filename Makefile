CFLAGS=-Wall -Werror 
EXEC=server

.PHONY=clean

all: $(EXEC)

clean:
	rm -fr *.o $(EXEC)
