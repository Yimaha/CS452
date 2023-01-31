

#pragma once
#define MAX_MAP_SIZE 128
#include "rpi.h"

// this may or may not be useful in the future, but it here
// note that this is not tested yet
template <typename T>
class Hashmap
{
public:

	Hashmap();
	~Hashmap();
	bool is_empty();
	bool is_full();
	void insert(int key, T data);
    void del(int key);
	T* get(int key); // ideally just pass reference

private:
    // only accept non-negative keys
    struct MapItem{
        int real_key;
        T t;
    }
	MapItem buffer[MAX_MAP_SIZE] = {{-1, T()}};
	int size;
};

template <typename T>
Hashmap<T>::Hashmap()
	: size { 0 }
{ }

template <typename T>
Hashmap<T>::~Hashmap()
{ }

template <typename T>
bool Hashmap<T>::is_empty()
{
	return size == 0;
}

template <typename T>
bool Hashmap<T>::is_full()
{
	return size == MAX_MAP_SIZE - 1;
}

template <typename T>
bool Hashmap<T>::insert(int key, T data)
{
    if (is_full()) {
       print("unable to insert\r\n", 18);   
       return false;
    } else {
        int hash = key % MAX_MAP_SIZE;
        while (buffer[hash].real_key != -1) {
            hash = (hash + 1) % MAX_MAP_SIZE;
        }
        buffer[hash] = {key, data};
        return true;
    }
}

template <typename T>
bool Hashmap<T>::del(int key)
{
    if (is_empty()) {
       print("unable to delete\r\n", 18);   
       return false;
    } else {
        int hash = key % MAX_MAP_SIZE;
        while (buffer[hash].real_key != key) {
            hash = (hash + 1) % MAX_MAP_SIZE;
        }
        buffer[hash] = {-1, T()};
        return true;
    }
}


template <typename T>
T* Hashmap<T>::get(int key)
{
    if (is_empty()) {
       print("it empty, why are you calling get?\r\n", 36);   
    } else {
        int hash = key % MAX_MAP_SIZE;
        while (buffer[hash].real_key != key) {
            hash = (hash + 1) % MAX_MAP_SIZE;
        }
        return &buffer[hash].t;
    }
}



