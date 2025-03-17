#include <iostream>
#include <stdexcept>

#ifndef MYVECTOR_H
#define MYVECTOR_H



// A custom vector class mimicking std::vector
template <typename T>
class MyVector {
private:
    T* data;        // Pointer to the dynamically allocated array
    long _size;     // Current size of the vector
    long _capacity; // Current capacity of the vector

    void reallocate(long new_capacity) {
        T* new_data = new T[new_capacity];
        for (long i = 0; i < _size; ++i) {
            new_data[i] = data[i];
        }
        delete[] data;
        data = new_data;
        _capacity = new_capacity;
    }

public:
    class Iterator {
    private:
        T* ptr;
    public:
        explicit Iterator(T* p) : ptr(p) {}

        T& operator*() { return *ptr; }
        const T& operator*() const { return *ptr; }

        Iterator& operator++() { ++ptr; return *this; }
        Iterator operator++(int) { Iterator temp = *this; ++(*this); return temp; }

        Iterator& operator--() { --ptr; return *this; }
        Iterator operator--(int) { Iterator temp = *this; --(*this); return temp; }

        bool operator==(const Iterator& other) const { return ptr == other.ptr; }
        bool operator!=(const Iterator& other) const { return ptr != other.ptr; }
    };

    MyVector() : data(nullptr), _size(0), _capacity(0) {}

    explicit MyVector(long count, const T& value = T()) 
        : data(new T[count]), _size(count), _capacity(count) {
        for (long i = 0; i < count; ++i) {
            data[i] = value;
        }
    }

    ~MyVector() {
        delete[] data;
    }

    MyVector(const MyVector& other)
        : data(new T[other._capacity]), _size(other._size), _capacity(other._capacity) {
        for (long i = 0; i < _size; ++i) {
            data[i] = other.data[i];
        }
    }

    MyVector& operator=(const MyVector& other) {
        if (this != &other) {
            delete[] data;
            _size = other._size;
            _capacity = other._capacity;
            data = new T[_capacity];
            for (long i = 0; i < _size; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    MyVector(MyVector&& other) noexcept
        : data(other.data), _size(other._size), _capacity(other._capacity) {
        other.data = nullptr;
        other._size = 0;
        other._capacity = 0;
    }

    MyVector& operator=(MyVector&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            _size = other._size;
            _capacity = other._capacity;

            other.data = nullptr;
            other._size = 0;
            other._capacity = 0;
        }
        return *this;
    }

    T& operator[](long index) {
        if (index >= _size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    const T& operator[](long index) const {
        if (index >= _size)  
            throw std::out_of_range("Index out of range");
        return data[index];
    }

    T& at(long index) {
        if (index >= _size)
            throw std::out_of_range("Index out of range");
        return data[index];
    }

    const T& at(long index) const {
        if (index >= _size)
            throw std::out_of_range("Index out of range");
        return data[index];
    }

    // Size and capacity
    long size() const {
        return _size;
    }

    long capacity() const {
        return _capacity;
    }

    bool empty() const {
        return _size == 0;
    }

    void push_back(const T& value) {
        if (_size == _capacity) {
            reallocate(_capacity == 0 ? 1 : _capacity * 2);
        }
        data[_size++] = value;
    }

    void pop_back() {
        if (_size > 0) {
            --_size;
        }
    }

    void clear() {
        _size = 0;
    }

    void resize(long new_size, const T& value = T()) {
        if (new_size > _capacity) {
            reallocate(new_size);
        }
        for (long i = _size; i < new_size; ++i) {
            data[i] = value;
        }
        _size = new_size;
    }

    Iterator begin() { return Iterator(data); }
    Iterator end() { return Iterator(data + _size); }
};

#endif