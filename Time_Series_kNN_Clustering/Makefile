CC=g++
CFLAGS=-Wall
OBJ=obj/hashFunctionH.o obj/hashFunctionG.o obj/randGen.o obj/hashTable.o obj/readInput.o obj/point.o obj/LSH.o obj/hamming.o obj/LSH.o obj/kmeans.o obj/cluster.o obj/hypercube.o obj/hashFunctionF.o obj/searchUtils.o obj/timeSeries.o
OBJDIR := obj

all: $(OBJDIR) search

safe: clean $(OBJDIR) search

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^

search: obj/search.o $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR):
	mkdir -p $(OBJDIR)

.PHONY: clean
clean:
	rm -f obj/*.o search *.txt
