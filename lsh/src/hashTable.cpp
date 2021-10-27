#include <iostream>
#include <string>
#include "../inc/hashTable.hpp"
#include "../inc/point.hpp"


HashTable::HashTable(unsigned int numBuckets) {
    this->numBuckets = numBuckets;
    this->hashArray = new std::list<Item *>[numBuckets];
}

HashTable::~HashTable() {
    std::list<Item *>::iterator it;
    for (unsigned int bucket = 0; bucket < this->numBuckets; bucket++) {
        for (it = this->hashArray[bucket].begin(); it != this->hashArray[bucket].end(); ++it) {
            delete (*it);
        }
    }
    delete[] (this->hashArray);
}

HashTable::HashTable(const HashTable &copy){
    this->numBuckets = copy.numBuckets;
    this->hashArray = new std::list<Item *>[numBuckets];
}

HashTable& HashTable::operator=(const HashTable &copy){
    this->numBuckets = copy.numBuckets;
    this->hashArray = new std::list<Item *>[this->numBuckets];
    return *this;
}




//Looks in bucket for ID==key, if found returns 1, otherwise 0
int HashTable::search(unsigned int bucket, unsigned int key) const {

    if (bucket >= this->numBuckets) {
        return -1;
    }

    for(unsigned int i=0; i<this->numBuckets; i++){
        std::list<Item *>::iterator it;

        for (it = this->hashArray[i].begin(); it != this->hashArray[i].end(); ++it) {
            if ((*it)->data->getId() == "1") {
                std::cout<<i<<std::endl;
                return 1;
            }
        }
    }


    return 0;
}

//Counts the amount of instances in bucket with ID==key
int HashTable::count(unsigned int bucket, unsigned int key) const {

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

//Creates a new entry in bucket with ID=key and a ptr to the data
int HashTable::insert(unsigned int bucket, unsigned int key, ITEM_TYPE *data) {

    if (bucket >= this->numBuckets) {
        return -1;
    }

    Item *newItem = new Item;
    newItem->key = key;
    newItem->data = data;

    this->hashArray[bucket].push_back(newItem);

    return 0;
}

//Removes only the first instance of key in bucket
int HashTable::remove(unsigned int bucket, unsigned int key) {

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

std::list<Item *>& HashTable::getBucket(unsigned int bucket){

    //cannot return NULL, maybe return pointer to list?
    //    if(bucket >= this->numBuckets){
    //        return NULL;
    //    }

//    else{
        return this->hashArray[bucket];
//    }
}


void HashTable::print() const {

    std::cout << "------------------------------------------" << std::endl;

    std::list<Item *>::iterator it;
    for (unsigned int bucket = 0; bucket < this->numBuckets; bucket++) {
        std::cout << "BUCKET " << bucket << " : ";
        for (it = this->hashArray[bucket].begin(); it != this->hashArray[bucket].end(); ++it) {
            std::cout << (*it)->key << " -> ";
            //(*it)->data->print();
        }
        std::cout << std::endl;
    }

    std::cout << "------------------------------------------" << std::endl;
}
