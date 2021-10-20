#include "../inc/hashTable.hpp"

HashTable::HashTable(){
    std::cout << "Default created" << std::endl;
}

HashTable::HashTable(unsigned int numBuckets) {
    this->numBuckets = numBuckets;
    this->hashArray = new std::list<Item *>[numBuckets];
    std::cout << "HashTable Created" << std::endl;
}

HashTable::~HashTable() {
    std::list<Item *>::iterator it;
    for (unsigned int bucket = 0; bucket < this->numBuckets; bucket++) {
        for (it = this->hashArray[bucket].begin(); it != this->hashArray[bucket].end(); ++it) {
            delete (*it);
        }
    }
    delete[] (this->hashArray);
    std::cout << "HashTable Deleted" << std::endl;
}

HashTable::HashTable(const HashTable &copy){
    this->numBuckets = copy.numBuckets;
    this->hashArray = new std::list<Item *>[numBuckets];
    std::cout << "HashTable Copy Created" << std::endl;
}

HashTable& HashTable::operator=(const HashTable &copy){
    std::cout << "HashTable Copy Opp Created" << std::endl;
    this->numBuckets = copy.numBuckets;
    this->hashArray = new std::list<Item *>[this->numBuckets];
    return *this;
}

int HashTable::search(unsigned int bucket, int key) const {

    if (bucket >= this->numBuckets) {
        return -1;
    }

    std::list<Item *>::iterator it;

    for (it = this->hashArray[bucket].begin(); it != this->hashArray[bucket].end(); ++it) {
        if ((*it)->key == key) {
            return 1;
        }
    }
    return 0;
}

int HashTable::count(unsigned int bucket, int key) const {

    if (bucket >= this->numBuckets) {
        return -1;
    }

    unsigned int count = 0;

    std::list<Item *>::iterator it;

    for (it = this->hashArray[bucket].begin(); it != this->hashArray[bucket].end(); ++it) {
        if ((*it)->key == key) {
            count++;
        }
    }
    return count;
}

int HashTable::insert(unsigned int bucket, int key, ITEM_TYPE *data) {

    if (bucket >= this->numBuckets) {
        return -1;
    }

    Item *newItem = new Item;
    newItem->key = key;
    newItem->data = data;

    this->hashArray[bucket].push_back(newItem);

    return 0;
}

int HashTable::remove(unsigned int bucket, int key) {

    if (bucket >= this->numBuckets) {
        return -1;
    }

    std::list<Item *>::iterator it;

    for (it = this->hashArray[bucket].begin(); it != this->hashArray[bucket].end(); ++it) {
        if ((*it)->key == key) {
            delete (*it);
            this->hashArray[bucket].remove(*it);
            return 1;
        }
    }

    return 0;
}

void HashTable::print() const {

    std::cout << "------------------------------------------" << std::endl;

    std::list<Item *>::iterator it;
    for (unsigned int bucket = 0; bucket < this->numBuckets; bucket++) {
        std::cout << "BUCKET " << bucket << " : ";
        for (it = this->hashArray[bucket].begin(); it != this->hashArray[bucket].end(); ++it) {
            std::cout << (*it)->key << " -> ";
        }
        std::cout << std::endl;
    }

    std::cout << "------------------------------------------" << std::endl;
}
