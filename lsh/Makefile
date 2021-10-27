CC=g++
CFLAGS=-Wall
BUILDNAME=lsh
OBJ=obj/main.o obj/hashFunctionH.o obj/hashFunctionG.o obj/randGen.o obj/hashTable.o obj/readInput.o obj/point.o obj/LSH.o
OBJDIR := obj

all: clean  $(OBJDIR) $(BUILDNAME)

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILDNAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR):
	mkdir -p $(OBJDIR)

.PHONY: clean
clean:
	rm -f obj/*.o $(BUILDNAME)
