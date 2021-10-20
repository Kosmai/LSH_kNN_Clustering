#pragma once

#include <list>
#include <string>
#include <iostream>

class Point;

#define ITEM_TYPE Point

struct Item {
    int key;
    ITEM_TYPE *data;
};

class HashTable {
private:
    unsigned int numBuckets;
    std::list<Item *> *hashArray;

public:
    HashTable();

    HashTable(const HashTable &copy);

    HashTable& operator=(const HashTable &copy);

    explicit HashTable(unsigned int);

    ~HashTable();

    int search(unsigned int, int) const;

    int count (unsigned int, int) const;

    int insert(unsigned int, int, ITEM_TYPE *);

    int remove(unsigned int, int);

    void print() const;
};
