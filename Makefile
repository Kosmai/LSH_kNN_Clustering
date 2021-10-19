CC=g++
CFLAGS=-Wall
BUILDNAME=lsh
DEPS=hashFunctionH.hpp
OBJ=main.o hashFunctionH.o

all: clean $(BUILDNAME)

%.o: %.c ${DEPS}
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILDNAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f *.o $(BUILDNAME)
