#pragma once

#include <list>
#include <string>
#include <iostream>

#define ITEM_TYPE int

struct Item {
    int key;
    ITEM_TYPE *data;
};

class Hashtable {
private:
    unsigned int numBuckets;
    std::list<Item *> *hashArray;

public:
    Hashtable();

    explicit Hashtable(unsigned int);

    ~Hashtable();

    int search(unsigned int, int) const;

    int count (unsigned int, int) const;

    int insert(unsigned int, int, ITEM_TYPE *);

    int remove(unsigned int, int);

    void print() const;
};
