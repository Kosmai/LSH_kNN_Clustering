#include "hashTable.hpp"

Hashtable::Hashtable(unsigned int numBuckets) {
    this->numBuckets = numBuckets;
    this->hashArray = new std::list<Item *>[numBuckets];
    std::cout << "Hashtable Created" << std::endl;
}

Hashtable::~Hashtable() {
    std::list<Item *>::iterator it;
    for (unsigned int bucket = 0; bucket < this->numBuckets; bucket++) {
        for (it = this->hashArray[bucket].begin(); it != this->hashArray[bucket].end(); ++it) {
            delete (*it);
        }
    }

    delete[] (this->hashArray);
    std::cout << "Hashtable Deleted" << std::endl;
}

int Hashtable::search(unsigned int bucket, int key) const {
    std::list<Item *>::iterator it;

    for (it = this->hashArray[bucket].begin(); it != this->hashArray[bucket].end(); ++it) {
        if ((*it)->key == key) {
            std::cout << "Item Found" << std::endl;
            return 1;
        }
    }

    std::cout << "Item Not Found" << std::endl;
    return 0;
}

int Hashtable::insert(unsigned int bucket, int key, ITEM_TYPE *data) {
    Item *newItem = new Item;
    newItem->key = key;
    newItem->data = data;

    this->hashArray[bucket].push_back(newItem);

    return 0;
}

int Hashtable::remove(unsigned int bucket, int key) {
    std::list<Item *>::iterator it;

    for (it = this->hashArray[bucket].begin(); it != this->hashArray[bucket].end(); ++it) {
        if ((*it)->key == key) {
            delete(*it);
            this->hashArray[bucket].remove(*it);
            return 1;
        }
    }

    return 0;
}

void Hashtable::print() const {

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
