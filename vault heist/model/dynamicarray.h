// cannot use vectors so were making our own hussain
#pragma once

template <typename T>
class DynArray {
private:
    T*  data;       // pointer to the memory that holds the items
    int count;      // how many items are stored right now
    int capacity;   // how many items fit before we must grow

    //aloocates new memory by copying old data into the new one and deleting the old memory so basically we have two shi
    void grow() {
        int newCapacity = capacity * 2;
        T* newData = new T[newCapacity];
        for (int i = 0; i < count; i++) {
            newData[i] = data[i];
        }
        delete[] data;          // free the old memory so it doesn't leak
        data = newData;
        capacity = newCapacity;
    }

public:
    // start w a size of 8
    DynArray() {
        capacity = 8;
        count = 0;
        data = new T[capacity];
    }

    // Runs automatically when the DynArray goes away: free the memory.
    ~DynArray() {
        delete[] data;
    }

    // Copy constructor: makes a brand new memory block with its own copy of the items.
    DynArray(DynArray& other) {
        capacity = other.capacity;
        count = other.count;
        data = new T[capacity];
        for (int i = 0; i < count; i++) {
            data[i] = other.data[i];
        }
    }

    // Copy assignment: a = b
    DynArray& operator=(DynArray& other) {
        if (this == &other) return *this;   // a = a: do nothing
        delete[] data;                      // free what we had
        capacity = other.capacity;
        count = other.count;
        data = new T[capacity];
        for (int i = 0; i < count; i++) {
            data[i] = other.data[i];
        }
        return *this;
    }

    // Add an item to the end.
    void push(T item) {
        if (count == capacity) {
            grow();
        }
        data[count] = item;
        count++;
    }

    // Remove and return the last item. On an empty array, returns a default value.
    T pop() {
        if (count == 0) {
            return T();
        }
        count--;
        return data[count];
    }

    // Access by index: a[3]
    T& operator[](int i) {
        return data[i];
    }

    int size() {
        return count;
    }

    bool empty() {
        return count == 0;
    }

    // Forget all items. The memory stays, so pushing again is fast.
    void clear() {
        count = 0;
    }
};