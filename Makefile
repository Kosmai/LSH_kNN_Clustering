CC=g++
CFLAGS=-Wall
BUILDNAME=lsh
OBJ=obj/main.o obj/hashFunctionH.o obj/hashFunctionG.o obj/randGen.o obj/hashTable.o obj/readInput.o obj/point.o obj/LSH.o

all: clean $(BUILDNAME)

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILDNAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f obj/*.o $(BUILDNAME)
