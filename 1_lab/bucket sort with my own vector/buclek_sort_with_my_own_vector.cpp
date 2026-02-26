#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <cstdint>
#include <limits>
#include <algorithm>   


template <typename T>
class Vector {
private:
    T* data_ptr = nullptr;
    size_t size = 0;
    size_t capacity = 0;

public:
    Vector() = default;

    Vector(const Vector& other) {
        if (this != &other) {
            resize(other.size);
            for (size_t i = 0; i < size; i++) {
                data_ptr[i] = other.data_ptr[i];
            }
        }
    }

    Vector(Vector&& other) noexcept
        : data_ptr(other.data_ptr), size(other.size), capacity(other.capacity) {
        other.data_ptr = nullptr;
        other.size = 0;
        other.capacity = 0;
    }

    ~Vector() {
        clear();
        ::operator delete(data_ptr);
    }

    Vector& operator=(const Vector& other) {
        if (this != &other) {
            clear();
            ::operator delete(data_ptr);

            data_ptr = static_cast<T*>(::operator new(other.capacity * sizeof(T)));
            size = other.size;
            capacity = other.capacity;

            for (size_t i = 0; i < size; i++) {
                new (&data_ptr[i]) T(other.data_ptr[i]);
            }
        }
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            clear();
            ::operator delete(data_ptr);

            data_ptr = other.data_ptr;
            size = other.size;
            capacity = other.capacity;

            other.data_ptr = nullptr;
            other.size = 0;
            other.capacity = 0;
        }
        return *this;
    }

    T& operator[](size_t index) {
        return data_ptr[index];
    }

    const T& operator[](size_t index) const {
        return data_ptr[index];
    }

    size_t sz() const {
        return size;
    }

    bool empty() const {
        return size == 0;
    }

    void clear() {
        for (size_t i = 0; i < size; i++) {
            data_ptr[i].~T();
        }
        size = 0;
    }

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity) return;

        T* new_vec = static_cast<T*>(::operator new(new_capacity * sizeof(T)));

        for (size_t i = 0; i < size; i++) {
            new (&new_vec[i]) T(std::move(data_ptr[i]));
            data_ptr[i].~T();
        }
        ::operator delete(data_ptr);
        data_ptr = new_vec;
        capacity = new_capacity;
    }

    void resize(size_t new_size) {
        if (new_size < size) {
            for (size_t i = new_size; i < size; i++) {
                data_ptr[i].~T();
            }
        } else if (new_size > size) {
            if (new_size > capacity) {
                reserve(std::max(new_size, capacity * 2));
            }
            for (size_t i = size; i < new_size; i++) {
                new (&data_ptr[i]) T();
            }
        }
        size = new_size;
    }

    template<typename U>
    
    void push_back(U &&val){
        if(size >= capacity){
            resize(capacity == 0 ? 1 : capacity * 2);
        }

        new (&data_ptr[size]) T(std::forward<U>(val));
        size++;
    }

    void pop_back(){
        if(size > 0){
            size--;
            data_ptr[size].~T();
        }
    }

    T* begin() { return data_ptr; }
    T* end() {return data_ptr + size; }

    const T* begin() const {return data_ptr; }
    const T* end() const {return data_ptr + size; }
};






using u = std::uint64_t;
using el = std::pair<u, Vector<char>>;

void bucket_sort(Vector<Vector<el>>& buck) {
    Vector<el> temp_data;
    u min_val = std::numeric_limits<u>::max();
    u max_val = std::numeric_limits<u>::min();
    el item;  
    while (std::cin >> item.first >> item.second) {
        temp_data.push_back(std::move(item));  
        if (temp_data[temp_data.sz() - 1].first < min_val) min_val = temp_data[temp_data.sz() - 1].first;
        if (temp_data[temp_data.sz() - 1].first > max_val) max_val = temp_data[temp_data.sz() - 1].first;
    }
    
    if (temp_data.empty()) return;
    u range = max_val - min_val;
    buck.clear();
    buck.resize(temp_data.sz());  
    
    auto get_ind = [&](u key) -> size_t {
        if (range == 0) return 0;
        double d = static_cast<double>(key - min_val) / range;
        return static_cast<size_t>(static_cast<double>(temp_data.sz() - 1) * d);
    };
    
    for (size_t i = 0; i < temp_data.sz(); i++) {
        size_t idx = get_ind(temp_data[i].first);
        buck[idx].push_back(std::move(temp_data[i]));
    }
}

void insertion_sort(Vector<el>& buck) {
    for (size_t i = 1; i < buck.sz(); i++) {
        u key = buck[i].first;
        Vector<char> val = std::move(buck[i].second);
        size_t left = 0;
        size_t right = i;     
        while (left < right) {
            size_t mid = left + (right - left) / 2;
            if (buck[mid].first <= key) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        
        if (left < i) {
            // std::move_backward(
            //     buck.begin() + left,
            //     buck.begin() + i,
            //     buck.begin() + i + 1
            // );
            for(size_t j = i; j > left; j--){
                buck[j] = std::move(buck[j - 1]);
            }
        }
        buck[left] = {key, std::move(val)};
    }
}

int main() {
    Vector<Vector<el>> buckets;      
    bucket_sort(buckets);
    for (auto& b : buckets) {
        if (b.sz() > 1) {
            insertion_sort(b);
        }
    }
    for (const auto& b : buckets) {
        for (const auto& p : b) {
            std::cout << p.first << "\t";
            
            for(size_t i = 0; i < p.second.sz(); i++){
                std::cout << p.second[i];
            }
            std::cout << '\n';
        }
    }
    return 0;
}