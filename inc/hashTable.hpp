#include <list>

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
    HashTable() = default;

    HashTable(const HashTable &copy);

    HashTable& operator=(const HashTable &copy);

    explicit HashTable(unsigned int);

    ~HashTable();


    //returns 1 if item found, 0 otherwise
    int search(unsigned int bucket, int key) const;

    //returns the amount of times "key" exists in bucket
    int count (unsigned int bucket, int key) const;

    //adds in bucket item with ID key and pointer to it
    int insert(unsigned int bucket, int key, ITEM_TYPE *);

    //deletes the first item with key found in bucket, returns 1/0
    int remove(unsigned int bucket, int key);

    void print() const;
};
