CC=g++
CFLAGS=-Wall
OBJ=obj/hashFunctionH.o obj/hashFunctionG.o obj/randGen.o obj/hashTable.o obj/readInput.o obj/point.o obj/LSH.o obj/hamming.o obj/LSH.o obj/kmeans.o obj/cluster.o obj/hypercube.o obj/hashFunctionF.o obj/searchUtils.o obj/timeSeries.o obj/clusteringUtils.o
FREDOBJ=fredobj/config.o fredobj/curve.o fredobj/frechet.o fredobj/fredPoint.o fredobj/interval.o fredobj/simplification.o 
OBJDIR := obj
FREDOBJDIR := fredobj

all: $(OBJDIR) $(FREDOBJDIR) search test cluster

safe: clean $(OBJDIR) $(FREDOBJDIR) search test cluster

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^

fredobj/%.o: fred/src/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^

test: obj/testing.o $(OBJ) $(FREDOBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lgtest -lpthread

search: obj/search.o $(OBJ) $(FREDOBJ)
	$(CC) $(CFLAGS) -o $@ $^

cluster: obj/clustering.o $(OBJ) $(FREDOBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(FREDOBJDIR):
	mkdir -p $(FREDOBJDIR)

.PHONY: clean
clean:
	rm -f obj/*.o fredobj/*.o search test clustering *.txt
