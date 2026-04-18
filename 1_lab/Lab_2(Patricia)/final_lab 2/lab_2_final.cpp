#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <algorithm>

using namespace std;

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
        swap(data, other.data);
        swap(len, other.len);
        return *this;
    }
    size_t size() const { return len; }
    const char* c_str() const { return data ? data : ""; }
    bool operator==(const String& other) const {
        if (len != other.len) return false;
        return strcmp(c_str(), other.c_str()) == 0;
    }
};

struct LatinDigitizer {
    bool operator()(const String& str, ptrdiff_t bitIdx) const noexcept {
        if (bitIdx < 0) return false;
        size_t charIdx = (size_t)bitIdx >> 3;
        if (charIdx >= str.size()) return false;
        return (str.c_str()[charIdx] >> (bitIdx & 7)) & 1;
    }
    ptrdiff_t operator()(const String& s1, const String& s2) const noexcept {
        size_t l1 = s1.size(), l2 = s2.size();
        size_t max_l = max(l1, l2);
        for (size_t i = 0; i < max_l; ++i) {
            unsigned char c1 = (i < l1) ? (unsigned char)s1.c_str()[i] : 0;
            unsigned char c2 = (i < l2) ? (unsigned char)s2.c_str()[i] : 0;
            if (c1 != c2) return (ptrdiff_t)(i * 8 + __builtin_ctz((unsigned char)(c1 ^ c2)));
        }
        return -1;
    }
};

template <class Key, class T, class Digitizer>
class PatriciaTree final {
private:
    struct Node {
        Node *left{}, *right{};
        Key key{};
        T value{};
        ptrdiff_t diffBit{-1};
    };
    
    struct NodeStack {
        static constexpr size_t CAPACITY = 8192;
        Node* items[CAPACITY];
        size_t top = 0;
        
        void push(Node* n) {
            if (top < CAPACITY) items[top++] = n;
        }
        Node* pop() {
            return (top > 0) ? items[--top] : nullptr;
        }
        bool empty() const { return top == 0; }
    };
    
    Node *root = nullptr;
    size_t count = 0;
    Digitizer digitizer{};

    void clear_iterative() {
        if (!root) return;
        NodeStack stack;
        stack.push(root);
        
        while (!stack.empty()) {
            Node* n = stack.pop();
            if (n->left && (root->diffBit == -1 || n->left->diffBit > n->diffBit)) {
                stack.push(n->left);
            } else if (n->right && (root->diffBit == -1 || n->right->diffBit > n->diffBit)) {
                stack.push(n->right);
            } else {
                delete n;
            }
        }
        root = nullptr;
        count = 0;
    }

public:
    PatriciaTree() = default;
    PatriciaTree(const Digitizer &d) : digitizer{d} {}
    ~PatriciaTree() { clear(); }

    void clear() {
        clear_iterative();
    }

    bool find(const char* k, T& result) const {
        if (!root) return false;
        String key(k);
        Node *c = root;
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
            root = new Node{nullptr, nullptr, key, val, -1};
            count = 1; 
            return true;
        }
        
        Node *p = nullptr, *c = root;
        while (c && c->diffBit != -1) {
            p = c;
            c = digitizer(key, c->diffBit) ? c->right : c->left;
        }
        if (!c) return false;
        if (c->key == key) return false;
        
        ptrdiff_t dbit = digitizer(key, c->key);
        if (dbit < 0) return false;
        
        
        Node* newLeaf = new Node{nullptr, nullptr, key, val, -1};
        Node* newInternal = new Node{nullptr, nullptr, String(""), 0, dbit};
        
        p = nullptr;
        Node* cur = root;
        
        while (cur && cur->diffBit != -1 && cur->diffBit < dbit) {
            p = cur;
            cur = digitizer(key, cur->diffBit) ? cur->right : cur->left;
        }
        
        if (!cur) {
            delete newLeaf;
            delete newInternal;
            return false;
        }
        
        
        if (!p) {
            if (digitizer(key, dbit)) { 
                newInternal->left = newLeaf;
                newInternal->right = cur;
            } else { 
                newInternal->right = newLeaf;
                newInternal->left = cur;
            }
            root = newInternal;
        } else {
            if (digitizer(key, dbit)) { 
                newInternal->right = newLeaf;
                newInternal->left = cur;
            } else { 
                newInternal->left = newLeaf;
                newInternal->right = cur;
            }
            if (digitizer(key, p->diffBit)) p->right = newInternal;
            else p->left = newInternal;
        }
        
        count++; 
        return true;
    }

    bool erase(const char* k) {
        if (!root) return false;
        String key(k);
        
        Node *p = nullptr, *c = root;
        while (c && c->diffBit != -1) {
            p = c;
            c = digitizer(key, c->diffBit) ? c->right : c->left;
        }
        if (!c || !(c->key == key)) return false;
        
        if (!p) {
            delete root;
            root = nullptr;
            count = 0;
            return true;
        }
        
        Node *sibling = (p->left == c) ? p->right : p->left;
        if (root == p) {
            root = sibling;
        } else {
            Node *gp = root;
            while (gp && (gp->left != p && gp->right != p)) {
                gp = digitizer(key, gp->diffBit) ? gp->right : gp->left;
            }
            if (!gp) return false;
            if (gp->left == p) gp->left = sibling;
            else gp->right = sibling;
        }
        delete c;
        delete p;
        count = (count > 0) ? count - 1 : 0;
        return true;
    }

    bool save(const char* path) const {
        ofstream f(path, ios::binary);
        if (!f) return false;
        f.write((char*)&count, sizeof(count));
        if (!root) return true;
        
        NodeStack stack;
        stack.push(root);
        
        while (!stack.empty()) {
            Node* n = stack.pop();
            
            if (n->diffBit == -1) {
                size_t l = n->key.size();
                f.write((char*)&l, sizeof(l));
                f.write(n->key.c_str(), l);
                f.write((char*)&n->value, sizeof(T));
            } else {
                if (n->right) stack.push(n->right);
                if (n->left) stack.push(n->left);
            }
        }
        return true;
    }

    bool load(const char* path) {
        ifstream f(path, ios::binary);
        if (!f) return false;
        size_t n; 
        if (!f.read((char*)&n, sizeof(n))) return false;
        clear();
        for (size_t i = 0; i < n; ++i) {
            size_t l; 
            if (!f.read((char*)&l, sizeof(l))) return false;
            char b[1024]; 
            if (!f.read(b, l)) return false;
            b[l] = '\0';
            T v; 
            if (!f.read((char*)&v, sizeof(T))) return false;
            insert(b, v);
        }
        return true;
    }
};

int main() {
    ios::sync_with_stdio(0); cin.tie(0);
    PatriciaTree<String, uint64_t, LatinDigitizer> tree;
    char cmd[512];
    while (cin >> cmd) {
        if (cmd[0] == '+') {
            char w[512]; uint64_t v; cin >> w >> v;
            cout << (tree.insert(w, v) ? "OK\n" : "Exist\n");
        } else if (cmd[0] == '-') {
            char w[512]; cin >> w;
            cout << (tree.erase(w) ? "OK\n" : "NoSuchWord\n");
        } else if (cmd[0] == '!') {
            char act[16], p[512]; cin >> act >> p;
            if (act[0] == 'S' || act[0] == 's') cout << (tree.save(p) ? "OK\n" : "ERROR\n");
            else cout << (tree.load(p) ? "OK\n" : "ERROR\n");
        } else {
            uint64_t r;
            if (tree.find(cmd, r)) cout << "OK: " << r << "\n";
            else cout << "NoSuchWord\n";
        }
    }
    return 0;
}
