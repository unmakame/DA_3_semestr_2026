#include <iostream>
#include <utility>
#include <cstdint>
#include <limits>

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
        if (this == &other) return *this;
        clear();
        ::operator delete(data_ptr);
        data_ptr = nullptr;
        if (other.capacity > 0) {
            data_ptr = static_cast<T*>(::operator new(other.capacity * sizeof(T)));
            capacity = other.capacity;
            size = other.size;
            for (size_t i = 0; i < size; ++i) {
                new (&data_ptr[i]) T(other.data_ptr[i]);
            }
        } else {
            capacity = 0;
            size = 0;
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

    T& operator[](size_t index) { return data_ptr[index]; }
    const T& operator[](size_t index) const { return data_ptr[index]; }

    size_t sz() const { return size; }
    bool empty() const { return size == 0; }

    void clear() {
        for (size_t i = 0; i < size; ++i) {
            data_ptr[i].~T();
        }
        size = 0;
    }

    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity) return;
        T* new_vec = static_cast<T*>(::operator new(new_capacity * sizeof(T)));
        for (size_t i = 0; i < size; ++i) {
            new (&new_vec[i]) T(std::move(data_ptr[i]));
            data_ptr[i].~T();
        }
        ::operator delete(data_ptr);
        data_ptr = new_vec;
        capacity = new_capacity;
    }

    void resize(size_t new_size) {
        if (new_size < size) {
            for (size_t i = new_size; i < size; ++i) {
                data_ptr[i].~T();
            }
        } else if (new_size > size) {
            if (new_size > capacity) {
                reserve(std::max(new_size, capacity == 0 ? 1 : capacity * 2));
            }
            for (size_t i = size; i < new_size; ++i) {
                new (&data_ptr[i]) T();
            }
        }
        size = new_size;
    }

    template<typename U>
    void push_back(U &&val){
        if(size >= capacity){
            reserve(capacity == 0 ? 1 : capacity * 2);
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
    T* end() { return data_ptr + size; }
    const T* begin() const { return data_ptr; }
    const T* end() const { return data_ptr + size; }
};

using u = std::uint64_t;
using el = std::pair<u, Vector<char>>;

void bucket_sort(Vector<el>& data) { //standart lib - vector not vector<vector> (input->sort->output)
    if (data.empty()) return;
    
    u min_val = std::numeric_limits<u>::max();
    u max_val = std::numeric_limits<u>::min();
    
    for (size_t i = 0; i < data.sz(); ++i) {
        if (data[i].first < min_val) min_val = data[i].first;
        if (data[i].first > max_val) max_val = data[i].first;
    }
    
    u range = max_val - min_val;
    if (range == 0) return;    
    
    size_t bucket_count = data.sz();
    Vector<Vector<el>> buckets;
    buckets.resize(bucket_count);
    
    for (size_t i = 0; i < data.sz(); ++i) {
        size_t bucket_index = static_cast<size_t>(
            ((static_cast<double>(data[i].first - min_val) / range) * (bucket_count - 1))
        );
        buckets[bucket_index].push_back(std::move(data[i]));
    }
    
    data.clear();
    
    for (size_t i = 0; i < buckets.sz(); ++i) {
        if (buckets[i].sz() > 1) {
            for (size_t j = 1; j < buckets[i].sz(); ++j) {
                el temp = std::move(buckets[i][j]);
                size_t k = j;
                while (k > 0 && buckets[i][k - 1].first > temp.first) {
                    buckets[i][k] = std::move(buckets[i][k - 1]);
                    --k;
                }
                buckets[i][k] = std::move(temp);
            }
        } 
        
        for (size_t j = 0; j < buckets[i].sz(); ++j) {
            data.push_back(std::move(buckets[i][j]));
        }
    }
}

int main() { 
    Vector<el> data;
    u key;
    char ch;
    
    while (std::cin >> key) {
        Vector<char> val;
        
        std::cin.get(ch);
        
        while (std::cin.get(ch)) {
            if (ch == '\n' || ch == '\r') break;
            val.push_back(ch);
        }
        data.push_back(el{key, std::move(val)});
    }
    
    bucket_sort(data);
    
    for (size_t i = 0; i < data.sz(); ++i) {
        std::cout << data[i].first << "\t";
        for (size_t j = 0; j < data[i].second.sz(); ++j) {
            std::cout << data[i].second[j];
        }
        std::cout << '\n';
    }
    return 0;
}


// - input data in main (STl)
// input-output in main // iter - nt vector
// binaty_insertion(sepparated) sort - and theory 