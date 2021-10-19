CC=g++
BUILDNAME=lsh
DEPS= 
OBJ=main.o

%.o: %.c ${DEPS}
	$(CC) -c -o $@ $<

$(BUILDNAME): $(OBJ)
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -f *.o $(BUILDNAME)
