// A small growable array used instead of an STL container.
#pragma once

template <typename T>
class DynArray {
private:
    T*  data;       // pointer to the memory that holds the items
    int count;      // how many items are stored right now
    int capacity;   // how many items fit before we must grow

    // Double the capacity while keeping all existing elements.
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
    // Start with room for eight elements.
    DynArray() {
        capacity = 8;
        count = 0;
        data = new T[capacity];
    }

    // Runs automatically when the DynArray goes away: free the memory.
    ~DynArray() {
        delete[] data;
    }

    // Copy the elements into independently owned storage.
    DynArray(DynArray& other) {
        capacity = other.capacity;
        count = other.count;
        data = new T[capacity];
        for (int i = 0; i < count; i++) {
            data[i] = other.data[i];
        }
    }

    // Replace this array's contents with a copy of another array.
    DynArray& operator=(DynArray& other) {
        if (this == &other) {
            return *this;
        }
        delete[] data;
        capacity = other.capacity;
        count = other.count;
        data = new T[capacity];
        for (int i = 0; i < count; i++) {
            data[i] = other.data[i];
        }
        return *this;
    }

    // Append an item, growing the array when it is full.
    void push(T item) {
        if (count == capacity) {
            grow();
        }
        data[count] = item;
        count++;
    }

    // Remove and return the last item, or a default value when empty.
    T pop() {
        if (count == 0) {
            return T();
        }
        count--;
        return data[count];
    }

    // Access an element by index.
    T& operator[](int i) {
        return data[i];
    }

    // Read an element without allowing it to be changed.
    const T& operator[](int i) const {
        return data[i];
    }

    int size() const {
        return count;
    }

    bool empty() const {
        return count == 0;
    }

    // Remove all elements while keeping the allocated storage.
    void clear() {
        count = 0;
    }
};