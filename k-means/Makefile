CC=g++
CFLAGS=-Wall
BUILDNAME=kmeans
OBJ= main.o readInput.o point.o randGen.o cluster.o

all: clean $(BUILDNAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILDNAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f *.o $(BUILDNAME)
