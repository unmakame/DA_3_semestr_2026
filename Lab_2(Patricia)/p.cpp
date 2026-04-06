#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>

template <typename T1, typename T2>
struct pair {
    T1 first;
    T2 second;
};

template <typename T>
class MyStack {
private:
    T* buffer = nullptr;
    size_t sz = 0;
    size_t cap = 0;
    void expand() {
        cap = (cap == 0) ? 8 : cap * 2;
        T* nb = new T[cap];
        for (size_t i = 0; i < sz; ++i) nb[i] = buffer[i];
        delete[] buffer;
        buffer = nb;
    }
public:
    MyStack() = default;
    ~MyStack() { delete[] buffer; }
    void push(T v) { if (sz == cap) expand(); buffer[sz++] = v; }
    void pop() { if (sz > 0) sz--; }
    T& top() { return buffer[sz - 1]; }
    bool empty() const { return sz == 0; }
};

class MyString {
private:
    char* data = nullptr;
    size_t len = 0;
public:
    MyString() : data(nullptr), len(0) {}
    MyString(const char* s) {
        if (s) {
            len = strlen(s);
            data = new char[len + 1];
            memcpy(data, s, len + 1);
            for (size_t i = 0; i < len; ++i)
                if (data[i] >= 'A' && data[i] <= 'Z') data[i] += 32;
        }
    }
    MyString(const MyString& other) {
        if (other.data) {
            len = other.len;
            data = new char[len + 1];
            memcpy(data, other.data, len + 1);
        }
    }
    MyString(MyString&& other) noexcept : data(other.data), len(other.len) {
        other.data = nullptr; other.len = 0;
    }
    ~MyString() { delete[] data; }
    MyString& operator=(const MyString& other) {
        if (this != &other) {
            delete[] data; data = nullptr; len = other.len;
            if (other.data) {
                data = new char[len + 1];
                memcpy(data, other.data, len + 1);
            }
        }
        return *this;
    }
    MyString& operator=(MyString&& other) noexcept {
        if (this != &other) {
            delete[] data; data = other.data; len = other.len;
            other.data = nullptr; other.len = 0;
        }
        return *this;
    }
    size_t size() const { return len; }
    const char* c_str() const { return data ? data : ""; }
    char operator[](size_t i) const { return data[i]; }
};

class LatinDigitizer final {
public:
    static constexpr int charBits = 8;
    bool operator()(const MyString& str, ptrdiff_t idx) const noexcept {
        ptrdiff_t charIdx = idx / charBits;
        if (charIdx >= (ptrdiff_t)str.size()) return false;
        return ((unsigned char)str[charIdx] >> (idx % charBits)) & 1;
    }
    ptrdiff_t operator()(const MyString& s1, const MyString& s2) const noexcept {
        size_t l1 = s1.size(), l2 = s2.size();
        size_t maxL = (l1 > l2) ? l1 : l2;
        for (size_t i = 0; i < maxL; ++i) {
            unsigned char c1 = (i < l1) ? (unsigned char)s1[i] : 0;
            unsigned char c2 = (i < l2) ? (unsigned char)s2[i] : 0;
            if (c1 != c2) {
                unsigned char diff = c1 ^ c2;
                int b = 0;
                while (!(diff & (1 << b))) ++b;
                return (ptrdiff_t)(i * charBits + b);
            }
        }
        return -1;
    }
};

template <class Key, class T, class Digitizer>
class PatriciaTree final {
private:
    struct Node final {
        Node *left{}, *right{};
        pair<Key, T> value{};
        ptrdiff_t diffBit{-1};
    };

    Node* root{};
    size_t treeSize{0};
    Digitizer digitizer{};

    void clear_iterative() {
        if (!root) return;
        MyStack<Node*> stack;
        stack.push(root);
        MyStack<Node*> to_delete;
        
        while (!stack.empty()) {
            Node* curr = stack.top();
            
            if (curr->left && curr->left != curr && (curr->diffBit == -1 || curr->left->diffBit > curr->diffBit)) {
                stack.push(curr->left);
                curr->left = nullptr;
            } else if (curr->right && curr->right != curr && (curr->diffBit == -1 || curr->right->diffBit > curr->diffBit)) {
                stack.push(curr->right);
                curr->right = nullptr;
            } else {
                stack.pop();
                to_delete.push(curr);
            }
        }
        
        while (!to_delete.empty()) {
            delete to_delete.top();
            to_delete.pop();
        }
        root = nullptr; 
        treeSize = 0;
    }

public:
    PatriciaTree(const Digitizer &digitizer = Digitizer{}) : digitizer{digitizer} {}
    ~PatriciaTree() { clear_iterative(); }

    bool insert(const char* k, T v) {
        Key key(k);
        if (!root) {
            root = new Node{nullptr, nullptr, {key, v}, -1};
            treeSize = 1; 
            return true;
        }
        Node *p = nullptr, *c = root;
        while (c->diffBit != -1) {
            p = c;
            c = digitizer(key, c->diffBit) ? c->right : c->left;
        }
        if (strcmp(c->value.first.c_str(), key.c_str()) == 0) return false;
        ptrdiff_t dbit = digitizer(key, c->value.first);
        Node* newNode = new Node{nullptr, nullptr, {key, v}, dbit};
        p = nullptr; 
        Node* cur = root;
        while (cur->diffBit != -1 && cur->diffBit < dbit) {
            p = cur;
            cur = digitizer(key, cur->diffBit) ? cur->right : cur->left;
        }
        if (digitizer(key, dbit)) { 
            newNode->right = cur; 
            newNode->left = nullptr; 
        } else { 
            newNode->left = cur; 
            newNode->right = nullptr; 
        }
        if (!p) root = newNode;
        else if (digitizer(key, p->diffBit)) p->right = newNode;
        else p->left = newNode;
        treeSize++; 
        return true;
    }

    bool find(const char* k, T& res) {
        if (!root) return false;
        Key key(k);
        Node *p = nullptr, *c = root;
        while (c->diffBit != -1) {
            p = c;
            c = digitizer(key, c->diffBit) ? c->right : c->left;
        }
        if (strcmp(c->value.first.c_str(), key.c_str()) == 0) {
            res = c->value.second; 
            return true;
        }
        return false;
    }

    bool erase(const char* k) {
        if (!root) return false;
        Key key(k);
        Node *p = nullptr, *c = root;
        while (c->diffBit != -1) {
            p = c;
            c = digitizer(key, c->diffBit) ? c->right : c->left;
        }
        if (strcmp(c->value.first.c_str(), key.c_str()) != 0) return false;
        
        if (!p) {
            delete root;
            root = nullptr;
            treeSize = 0;
            return true;
        }

        Node *sibling = (p->left == c) ? p->right : p->left;
        Node *gp = nullptr, *temp = root;
        while (temp != p) {
            if (temp->diffBit == -1) break;
            gp = temp;
            temp = digitizer(key, temp->diffBit) ? temp->right : temp->left;
        }

        if (!gp) {
            root = sibling;
        } else {
            if (gp->left == p) gp->left = sibling;
            else gp->right = sibling;
        }

        delete c;
        delete p;
        treeSize--;
        return true;
    }

    bool save(const char* path) const {
        std::ofstream f(path, std::ios::binary);
        if (!f) return false;
        f.write((char*)&treeSize, sizeof(treeSize));
        if (!root) return true;
        MyStack<Node*> stack; stack.push(root);
        while (!stack.empty()) {
            Node* n = stack.top(); stack.pop();
            bool isLeaf = true;
            if (n->left && n->left->diffBit > n->diffBit) isLeaf = false;
            if (n->right && n->right->diffBit > n->diffBit) isLeaf = false;
            if (isLeaf) {
                size_t l = n->value.first.size();
                f.write((char*)&l, sizeof(l));
                f.write(n->value.first.c_str(), l);
                f.write((char*)&n->value.second, sizeof(T));
            } else {
                if (n->right && n->right->diffBit > n->diffBit) stack.push(n->right);
                if (n->left && n->left->diffBit > n->diffBit) stack.push(n->left);
            }
        }
        return true;
    }

    bool load(const char* path) {
        std::ifstream f(path, std::ios::binary);
        if (!f) return false;
        size_t ns; if (!f.read((char*)&ns, sizeof(ns))) return false;
        clear_iterative();
        for (size_t i = 0; i < ns; ++i) {
            size_t l; f.read((char*)&l, sizeof(l));
            char* buf = new char[l + 1];
            f.read(buf, l); buf[l] = '\0';
            T v; f.read((char*)&v, sizeof(T));
            insert(buf, v);
            delete[] buf;
        }
        return true;
    }
};

int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    PatriciaTree<MyString, uint64_t, LatinDigitizer> tree;
    char cmd[1024];
    while (std::cin >> cmd) {
        if (cmd[0] == '+') {
            char w[1024]; uint64_t v; std::cin >> w >> v;
            if (tree.insert(w, v)) std::cout << "OK\n"; else std::cout << "Exist\n";
        } else if (cmd[0] == '-') {
            char w[1024]; std::cin >> w;
            if (tree.erase(w)) std::cout << "OK\n"; else std::cout << "NoSuchWord\n";
        } else if (cmd[0] == '!') {
            char act[16], path[1024]; std::cin >> act >> path;
            if (act[0] == 'S' || act[0] == 's') {
                if (tree.save(path)) std::cout << "OK\n"; else std::cout << "ERROR\n";
            } else {
                if (tree.load(path)) std::cout << "OK\n"; else std::cout << "ERROR\n";
            }
        } else {
            uint64_t r;
            if (tree.find(cmd, r)) std::cout << "OK: " << r << "\n";
            else std::cout << "NoSuchWord\n";
        }
    }
    return 0;
}