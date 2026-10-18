#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>


template <class T>
class Vector {
private:
    T* data = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;

    void resize_capacity(size_t new_capacity) {
        if (new_capacity <= capacity_) return;
        T* new_data = new T[new_capacity];
        if (data) {
            for (size_t i = 0; i < size_; ++i) {
                new_data[i] = data[i];
            }
            delete[] data;
        }
        data = new_data;
        capacity_ = new_capacity;
    }

public:
    Vector() = default;
    ~Vector() { delete[] data; }
    
    Vector(const Vector& other) : size_(other.size_), capacity_(other.capacity_) {
        if (other.data) {
            data = new T[capacity_];
            for (size_t i = 0; i < size_; ++i) {
                data[i] = other.data[i];
            }
        }
    }
    
    Vector(Vector&& other) noexcept : data(other.data), size_(other.size_), capacity_(other.capacity_) {
        other.data = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    
    Vector& operator=(Vector other) {
        T* tmp = data;
        size_t tmp_size = size_;
        size_t tmp_cap = capacity_;
        data = other.data;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data = tmp;
        other.size_ = tmp_size;
        other.capacity_ = tmp_cap;
        return *this;
    }
    
    void push_back(const T& value) {
        if (size_ >= capacity_) {
            size_t new_cap = (capacity_ == 0) ? 10 : capacity_ * 2;
            resize_capacity(new_cap);
        }
        data[size_++] = value;
    }
    
    void push_back(T&& value) {
        if (size_ >= capacity_) {
            size_t new_cap = (capacity_ == 0) ? 10 : capacity_ * 2;
            resize_capacity(new_cap);
        }
        data[size_++] = static_cast<T&&>(value);
    }
    
    T& operator[](size_t index) { return data[index]; }
    const T& operator[](size_t index) const { return data[index]; }
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    void clear() { size_ = 0; }
    
    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) return;
        resize_capacity(new_capacity);
    }
    
    void reverse() {
        for (size_t i = 0; i < size_ / 2; ++i) {
            T tmp = data[i];
            data[i] = data[size_ - 1 - i];
            data[size_ - 1 - i] = tmp;
        }
    }
    
    T* begin() { return data; }
    T* end() { return data + size_; }
    const T* begin() const { return data; }
    const T* end() const { return data + size_; }
};

class String {
    char* data = nullptr;
    size_t len = 0;
public:
    String() = default;
    String(const char* str) {
        if (!str) return;
        len = strlen(str);
        data = new char[len + 1];
        for (size_t i = 0; i < len; ++i) {
            data[i] = (str[i] >= 'A' && str[i] <= 'Z') ? (char)(str[i] + 32) : str[i];
        }
        data[len] = '\0';
    }
    String(const String& other) {
        if (other.data) {
            len = other.len;
            data = new char[len + 1];
            memcpy(data, other.data, len + 1);
        }
    }
    String(String&& other) noexcept : data(other.data), len(other.len) {
        other.data = nullptr; other.len = 0;
    }
    ~String() { delete[] data; }
    String& operator=(String other) {
        char* tmp = data;
        size_t tmp_len = len;
        data = other.data;
        len = other.len;
        other.data = tmp;
        other.len = tmp_len;
        return *this;
    }
    size_t size() const { return len; }
    const char* c_str() const { return data ? data : ""; }
    bool operator==(const String& other) const {
        if (len != other.len) return false;
        return strcmp(c_str(), other.c_str()) == 0;
    }
    bool operator!=(const String& other) const {
        return !(*this == other);
    }
};

struct LatinDigitizer {
    static constexpr int CHAR_BITS = 5;
    
    bool operator()(const String& str, ptrdiff_t idx) const noexcept {
        if (idx < 0) return false;
        size_t charIdx = (size_t)idx / CHAR_BITS;
        if (charIdx >= str.size()) return false;
        int offset = CHAR_BITS - 1 - ((size_t)idx % CHAR_BITS);
        return (str.c_str()[charIdx] >> offset) & 1;
    }
    
    ptrdiff_t operator()(const String& s1, const String& s2) const noexcept {
        size_t l1 = s1.size(), l2 = s2.size();
        size_t max_l = (l1 > l2) ? l1 : l2;
        for (size_t i = 0; i < max_l; ++i) {
            unsigned char c1 = (i < l1) ? (unsigned char)s1.c_str()[i] : 0;
            unsigned char c2 = (i < l2) ? (unsigned char)s2.c_str()[i] : 0;
            if (c1 != c2) {
                for (int b = 0; b < CHAR_BITS; ++b) {
                    int offset = CHAR_BITS - 1 - b;
                    if (((c1 >> offset) & 1) != ((c2 >> offset) & 1)) {
                        return (ptrdiff_t)(i * CHAR_BITS + b);
                    }
                }
            }
        }
        return -1;
    }
};

template <class Key, class T, class Digitizer>
class PatriciaTree final {
private:
    struct Node {
        Node *left = nullptr, *right = nullptr;
        Key key{};
        T value{};
        ptrdiff_t diffBit = -1;
        int id = -1;
    };

    Node* root = nullptr;
    size_t sz = 0;
    Digitizer digitizer{};

public:
    PatriciaTree() = default;
    PatriciaTree(const Digitizer& d) : digitizer{d} {}
    ~PatriciaTree() { clear(); }

    void clear() {
        if (!root) return;
        Vector<Node*> toDelete;
        toDelete.push_back(root);
        
        for (size_t idx = 0; idx < toDelete.size(); ++idx) {
            Node* n = toDelete[idx];
            if (n->left && n->left->diffBit > n->diffBit) {
                toDelete.push_back(n->left);
            }
            if (n->right && n->right->diffBit > n->diffBit) {
                toDelete.push_back(n->right);
            }
        }
        
        for (size_t idx = 0; idx < toDelete.size(); ++idx) {
            delete toDelete[idx];
        }
        
        root = nullptr;
        sz = 0;
    }

    bool find(const char* k, T& result) const {
        if (!root) return false;
        String key(k);
        Node* c = root;
        while (c && c->diffBit != -1) {
            c = digitizer(key, c->diffBit) ? c->right : c->left;
        }
        if (!c) return false;
        if (c->key == key) {
            result = c->value;
            return true;
        }
        return false;
    }

    bool insert(const char* k, T val) {
        String key(k);
        if (!root) {
            root = new Node();
            root->key = key;
            root->value = val;
            root->diffBit = -1;
            root->left = root;
            root->right = root;
            ++sz;
            return true;
        }
        
        Node* p = nullptr;
        Node* c = root;
        while (c && c->diffBit != -1) {
            p = c;
            c = digitizer(key, c->diffBit) ? c->right : c->left;
        }
        if (!c) return false;
        if (c->key == key) return false;
        
        ptrdiff_t dbit = digitizer(key, c->key);
        if (dbit < 0) return false;
        
        Node* newLeaf = new Node();
        newLeaf->key = key;
        newLeaf->value = val;
        newLeaf->diffBit = -1;
        newLeaf->left = newLeaf;
        newLeaf->right = newLeaf;
        
        Node* newInternal = new Node();
        newInternal->diffBit = dbit;
        
        if (digitizer(key, dbit)) {
            newInternal->right = newLeaf;
            newInternal->left = c;
        } else {
            newInternal->left = newLeaf;
            newInternal->right = c;
        }
        
        if (!p) {
            root = newInternal;
        } else {
            if (digitizer(key, p->diffBit)) p->right = newInternal;
            else p->left = newInternal;
        }
        
        ++sz;
        return true;
    }

    bool erase(const char* k) {
        if (!root) return false;
        String key(k);
        
        Node* p = nullptr;
        Node* c = root;
        while (c && c->diffBit != -1) {
            p = c;
            c = digitizer(key, c->diffBit) ? c->right : c->left;
        }
        if (!c || !(c->key == key)) return false;
        
        if (!p) {
            delete root;
            root = nullptr;
            sz = 0;
            return true;
        }
        
        Node* sibling = (p->left == c) ? p->right : p->left;
        if (root == p) {
            root = sibling;
        } else {
            Node* gp = root;
            while (gp && (gp->left != p && gp->right != p)) {
                gp = digitizer(key, gp->diffBit) ? gp->right : gp->left;
            }
            if (!gp) return false;
            if (gp->left == p) gp->left = sibling;
            else gp->right = sibling;
        }
        delete c;
        delete p;
        sz = (sz > 0) ? sz - 1 : 0;
        return true;
    }

    bool save(const char* path) const {
        std::ofstream f(path, std::ios::binary);
        if (!f) return false;
        
        if (!root) {
            size_t zero = 0;
            f.write((char*)&zero, sizeof(zero));
            return true;
        }

        Vector<Node*> queue;
        queue.push_back(root);
        root->id = 0;
        for (size_t qidx = 0; qidx < queue.size(); ++qidx) {
            Node* n = queue[qidx];
            
            if (n->diffBit != -1) {
                if (n->left && n->left->id == -1) {
                    n->left->id = (int)queue.size();
                    queue.push_back(n->left);
                }
                if (n->right && n->right->id == -1) {
                    n->right->id = (int)queue.size();
                    queue.push_back(n->right);
                }
            }
        }

        size_t nodeNum = queue.size();
        f.write((char*)&nodeNum, sizeof(nodeNum));

        for (size_t idx = 0; idx < queue.size(); ++idx) {
            Node* n = queue[idx];
            f.write((char*)&n->diffBit, sizeof(n->diffBit));

            if (n->diffBit == -1) {
                size_t len = n->key.size();
                f.write((char*)&len, sizeof(len));
                f.write(n->key.c_str(), len);
                f.write((char*)&n->value, sizeof(T));
            } else {
                if (!n->left || !n->right) return false;
                int leftId = n->left->id;
                int rightId = n->right->id;
                f.write((char*)&leftId, sizeof(leftId));
                f.write((char*)&rightId, sizeof(rightId));
            }
        }

        for (size_t i = 0; i < queue.size(); ++i) {
            queue[i]->id = -1;
        }

        return true;
    }

    bool load(const char* path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        clear();
        sz = 0;
        return true;
    }

    size_t nodeNum = 0;
    if (!f.read((char*)&nodeNum, sizeof(nodeNum))) {
        clear();
        sz = 0;
        return false;
    }

    clear();
    if (nodeNum == 0) {
        root = nullptr;
        return true;
    }

    Vector<Node*> nodes;
    nodes.reserve(nodeNum);

    
    Vector<Vector<Node*>> waitLeft;
    Vector<Vector<Node*>> waitRight;
    waitLeft.reserve(nodeNum);
    waitRight.reserve(nodeNum);

    for (size_t i = 0; i < nodeNum; ++i) {
        waitLeft.push_back(Vector<Node*>());
        waitRight.push_back(Vector<Node*>());
    }

    for (size_t i = 0; i < nodeNum; ++i) {
        Node* newNode = new Node();
        nodes.push_back(newNode);

        ptrdiff_t db = 0;
        if (!f.read((char*)&db, sizeof(db))) return false;
        newNode->diffBit = db;

        if (db == -1) {
            size_t len = 0;
            if (!f.read((char*)&len, sizeof(len)) || len >= 1024) return false;

            char buf[1024];
            if (!f.read(buf, len)) return false;
            buf[len] = '\0';

            newNode->key = String(buf);

            if (!f.read((char*)&newNode->value, sizeof(T))) return false;

            newNode->left = newNode;
            newNode->right = newNode;
            ++sz;
        } else {
            int leftId = -1, rightId = -1;
            if (!f.read((char*)&leftId, sizeof(leftId)) ||
                !f.read((char*)&rightId, sizeof(rightId))) return false;

           
            if (leftId >= 0 && leftId < (int)nodeNum) {
                if (leftId < (int)nodes.size()) {
                    newNode->left = nodes[leftId];
                } else {
                    waitLeft[leftId].push_back(newNode);
                }
            } else {
                newNode->left = nullptr;
            }

           
            if (rightId >= 0 && rightId < (int)nodeNum) {
                if (rightId < (int)nodes.size()) {
                    newNode->right = nodes[rightId];
                } else {
                    waitRight[rightId].push_back(newNode);
                }
            } else {
                newNode->right = nullptr;
            }
        }

       
        for (size_t j = 0; j < waitLeft[i].size(); ++j) {
            waitLeft[i][j]->left = newNode;
        }
        for (size_t j = 0; j < waitRight[i].size(); ++j) {
            waitRight[i][j]->right = newNode;
        }
    }

    root = nodes[0];
    return true;
	}
};

int main() {
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);

    PatriciaTree<String, uint64_t, LatinDigitizer> tree;
    char cmd[512];

    while (std::cin >> cmd) {
        if (cmd[0] == '+') {
            char w[512];
            uint64_t v;
            std::cin >> w >> v;
            std::cout << (tree.insert(w, v) ? "OK\n" : "Exist\n");
        } else if (cmd[0] == '-') {
            char w[512];
            std::cin >> w;
            std::cout << (tree.erase(w) ? "OK\n" : "NoSuchWord\n");
        } else if (cmd[0] == '!') {
            char act[16], p[512];
            std::cin >> act >> p;
            if (act[0] == 'S' || act[0] == 's') {
                std::cout << (tree.save(p) ? "OK\n" : "ERROR\n");
            } else {
                std::cout << (tree.load(p) ? "OK\n" : "ERROR\n");
            }
        } else {
            uint64_t r;
            if (tree.find(cmd, r)) {
                std::cout << "OK: " << r << "\n";
            } else {
                std::cout << "NoSuchWord\n";
            }
        }
    }

    return 0;
}
