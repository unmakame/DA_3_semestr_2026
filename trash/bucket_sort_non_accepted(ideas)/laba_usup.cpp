#include <iostream>

template <class T>
class MyVector
{
private:
    size_t sz;
    size_t capacity;
    T* data = nullptr;

public:
    class iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
    private:
        pointer m_ptr;
    public:
        iterator(pointer ptr = nullptr) : m_ptr(ptr) {}
        reference operator*() const { return *m_ptr; }
        pointer operator->() const { return m_ptr; }
        iterator& operator++() { ++m_ptr; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
        iterator& operator--() { --m_ptr; return *this; }
        iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }
        iterator& operator+=(difference_type n) { m_ptr += n; return *this; }
        iterator operator+(difference_type n) const { iterator tmp = *this; tmp += n; return tmp; }
        iterator& operator-=(difference_type n) { m_ptr -= n; return *this; }
        iterator operator-(difference_type n) const { iterator tmp = *this; tmp -= n; return tmp; }
        difference_type operator-(const iterator& other) const { return m_ptr - other.m_ptr; }
        reference operator[](difference_type n) const { return *(m_ptr + n); }
        bool operator==(const iterator& other) const { return m_ptr == other.m_ptr; }
        bool operator!=(const iterator& other) const { return !(*this == other); }
        bool operator<(const iterator& other) const { return m_ptr < other.m_ptr; }
        bool operator>(const iterator& other) const { return other < *this; }
        bool operator<=(const iterator& other) const { return !(*this > other); }
        bool operator>=(const iterator& other) const { return !(*this < other); }
    };
    class const_iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = const T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;
    private:
        pointer m_ptr;
    public:
        const_iterator(pointer ptr = nullptr) : m_ptr(ptr) {}
        const_iterator(const iterator& it) : m_ptr(it.m_ptr) {}
        reference operator*() const { return *m_ptr; }
        pointer operator->() const { return m_ptr; }
        const_iterator& operator++() { ++m_ptr; return *this; }
        const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }
        const_iterator& operator--() { --m_ptr; return *this; }
        const_iterator operator--(int) { const_iterator tmp = *this; --(*this); return tmp; }
        const_iterator& operator+=(difference_type n) { m_ptr += n; return *this; }
        const_iterator operator+(difference_type n) const { const_iterator tmp = *this; tmp += n; return tmp; }
        const_iterator& operator-=(difference_type n) { m_ptr -= n; return *this; }
        const_iterator operator-(difference_type n) const { const_iterator tmp = *this; tmp -= n; return tmp; }
        difference_type operator-(const const_iterator& other) const { return m_ptr - other.m_ptr; }
        reference operator[](difference_type n) const { return *(m_ptr + n); }
        bool operator==(const const_iterator& other) const { return m_ptr == other.m_ptr; }
        bool operator!=(const const_iterator& other) const { return !(*this == other); }
        bool operator<(const const_iterator& other) const { return m_ptr < other.m_ptr; }
        bool operator>(const const_iterator& other) const { return other < *this; }
        bool operator<=(const const_iterator& other) const { return !(*this > other); }
        bool operator>=(const const_iterator& other) const { return !(*this < other); }
    };
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
    ~MyVector() noexcept {
        delete[] data;
    	data = nullptr;
    	sz = 0;
    	capacity = 0;
    }
    
    void pop()
    {
        if (sz > 0) sz--;
    }
    
    void clear() {
        delete[] data;
    	data = nullptr;
    	sz = 0;
    	capacity = 0;
    }

    void push_back(const T& n)
    {
        if (sz >= capacity) {
            reallocate(2 * capacity);
        }
        data[sz] = n;
        sz++;
    }
    
    void push_back(T&& n)
    {
        if (sz >= capacity) {
            reallocate(2 * capacity);
        }
        data[sz] = std::move(n);
        sz++;
    }
    
    MyVector() 
        : sz(0), capacity(0), data(nullptr){};
    
    MyVector(const size_t& n) 
        : sz(n), capacity(n), data(new T[n]) {}

    MyVector(const size_t& n, const T& elem) 
        : sz(n), capacity(n), data(new T[n]) {
            for (size_t i = 0; i < n; ++i) {
                data[i] = elem;
            }
        }
    
    MyVector(const std::initializer_list<T>& t)
        : sz(0), capacity(t.size()), data(new T[t.size()])
    {
        for (const T& i : t) {
            push_back(i);
        }
    }
    
    MyVector(const MyVector<T>& other)
        : sz(other.sz), capacity(other.capacity), data(new T[other.capacity])
    {
        std::copy(other.data, other.data + other.sz, data);
    }
    
    MyVector(MyVector<T>&& other) noexcept
        : sz(other.sz), capacity(other.capacity), data(other.data)
    {
        other.data = nullptr;
        other.sz = 0;
        other.capacity = 0;
    }
    
    void erase(size_t ind) {
        for (size_t i = ind; i < sz - 1; ++i) {
            data[i] = data[i + 1];
        }
        sz--;
    }
    
    void reallocate(size_t newsize) {
        if (newsize == 0) newsize = 1;
        T* newdata = new T[newsize];
        if (data) {
            for (size_t i = 0; i < sz; ++i) {
                newdata[i] = std::move(data[i]);
            }
            delete[] data;
        }
        data = std::move(newdata);
        capacity = newsize;
    }
    
    void resize(size_t newsize) {
        if (newsize > sz) {
            reallocate(newsize * 2);
        }
        sz = newsize;
    }
    
    T& operator[](size_t ind) {
        return data[ind];
    }
    
    const T& operator[](size_t ind) const {
        return data[ind];
    }
    
    const size_t size() const {
        return sz;
    }
    
    MyVector<T>& operator=(const MyVector<T>& other) {
        if (this == &other) {
            return *this;
        }
        delete[] data;
    
        capacity = other.capacity;
        sz = other.sz;
        data = new T[capacity];
    
        std::copy(other.data, other.data + other.sz, data);
    
        return *this;
    }

    MyVector<T>& operator=(MyVector<T>&& other) {
        if (this != &other) {
            delete[] data;
            data = other.data;
            sz = other.sz;
            capacity = other.capacity;
            other.data = nullptr;
            other.sz = 0;
            other.capacity = 0;
        }
        return *this;
    }

    iterator begin() noexcept { return iterator(data); }
    iterator end() noexcept { return iterator(data + sz); }
    const_iterator begin() const noexcept { return const_iterator(data); }
    const_iterator end() const noexcept { return const_iterator(data + sz); }
    const_iterator cbegin() const noexcept { return const_iterator(data); }
    const_iterator cend() const noexcept { return const_iterator(data + sz); }
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }
    
};

using String = MyVector<char>;

struct node {
    String key;
    String val;
};

std::size_t find(const String& line, char c, std::size_t start = 0) {
    for (std::size_t i = start; i < line.size(); ++i) {
        if (line[i] == c) {
            return i;
        }
    }
    return line.size();
}

String substr(const String& line, std::size_t start, std::size_t end) {
    String result;
    for (std::size_t i = start; i < end && i < line.size(); ++i) {
        result.push_back(line[i]);
    }
    return result;
}

int stoi(const String& line) {
    int result = 0;
    for (std::size_t i = 0; i < line.size(); ++i) {
        result = result * 10 + (line[i] - '0');
    }
    return result;
}

int date_to_numb(const String& line) {
    size_t dot_1 = find(line, '.');
    size_t dot_2 = find(line, '.', dot_1 + 1);
    if (dot_1 >= line.size() || dot_2 >= line.size()) {
        return 0;
    }
    int day = stoi(substr(line, 0, dot_1));
    int month = stoi(substr(line, dot_1 + 1, dot_2));
    int year = stoi(substr(line, dot_2 + 1, line.size()));

    return year * 10000 + month * 100 + day;
}

template<typename iterator>
void radix_sort(iterator begin, iterator end) {
    if (begin == end) return;

    size_t n = std::distance(begin, end);
    MyVector<int> keys(n);
    
    auto it = begin;
    int max_val = 0;
    for (size_t i = 0; i < n; i++) {
        keys[i] = date_to_numb(it->key);
        if (keys[i] > max_val) max_val = keys[i];
        ++it;
    }

    MyVector<node> temp_nodes(n);
    MyVector<int> temp_keys(n);

    int count[10] = {0};

    for (int exp = 1; max_val / exp > 0; exp *= 10) {

        for (size_t i = 0; i < 10; ++i) {
            count[i] = 0;
        }

        for (size_t i = 0; i < n; ++i)
            count[(keys[i] / exp) % 10]++;

        for (size_t i = 1; i < 10; ++i)
            count[i] += count[i - 1];

        for (int i = n - 1; i >= 0; --i) {
            size_t digit = (keys[i] / exp) % 10;
            size_t pos = --count[digit];
            temp_nodes[pos] = std::move(*(begin + i));
            temp_keys[pos] = keys[i];
        }

        for (size_t i = 0; i < n; ++i) {
            *(begin + i) = std::move(temp_nodes[i]);
            keys[i] = temp_keys[i];
        }
    }
}

std::istream& operator>>(std::istream& is, String& s) {    
    char c;
    while (is.get(c) && std::isspace(c)); 
    if (!is) return is;
    s.clear();
    s.push_back(c);
    while(is.get(c) && !std::isspace(c)) {
        s.push_back(c);
    }
    return is;
}

std::ostream& operator<<(std::ostream& os, const String& s) {
    for (char c : s) {
        os << c;
    }
    return os;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    MyVector<node> v;
    node elem;
    while (std::cin >> elem.key >> elem.val) {
        v.push_back(std::move(elem));
    }

    radix_sort(v.begin(), v.end());

    for (const auto& item : v) {
        std::cout << item.key << '\t' << item.val << '\n';
    }
    
    return 0;
}