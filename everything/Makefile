CC=g++
CFLAGS=-Wall
OBJ=obj/hashFunctionH.o obj/hashFunctionG.o obj/randGen.o obj/hashTable.o obj/readInput.o obj/point.o obj/LSH.o obj/hamming.o obj/LSH.o obj/kmeans.o obj/cluster.o obj/hypercube.o obj/hashFunctionF.o
OBJDIR := obj

all: $(OBJDIR) lsh cube cluster

safe: clean $(OBJDIR) lsh cube cluster

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^

cube: obj/hyperMain.o $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

cluster: obj/clusteringMain.o $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

lsh: obj/LSHMain.o $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR):
	mkdir -p $(OBJDIR)

.PHONY: clean
clean:
	rm -f obj/*.o cube cluster lsh
