#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>

class String {
private:
    char* data = nullptr;
    size_t len = 0;
public:
    String() = default;
    String(const char* str) {
        if (!str) return;
        len = strlen(str);
        data = new char[len + 1];
        memcpy(data, str, len + 1);
    }
    String(const String& other) {
        if (other.data) {
            len = other.len;
            data = new char[len + 1];
            memcpy(data, other.data, len + 1);
        }
    }
    String(String&& other) noexcept : data(other.data), len(other.len) {
        other.data = nullptr;
        other.len = 0;
    }
    ~String() { delete[] data; }
    String& operator=(const String& other) {
        if (this != &other) {
            delete[] data;
            data = nullptr;
            len = other.len;
            if (other.data) {
                data = new char[len + 1];
                memcpy(data, other.data, len + 1);
            }
        }
        return *this;
    }
    String& operator=(String&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            len = other.len;
            other.data = nullptr;
            other.len = 0;
        }
        return *this;
    }
    size_t size() const { return len; }
    const char* c_str() const { return data ? data : ""; }
};

struct LatinDigitizer {
    static bool get_bit(const char* str, size_t len, int bitIdx) {
        int charIdx = bitIdx >> 3;
        if (charIdx >= (int)len) return false;
        return (str[charIdx] >> (bitIdx & 7)) & 1;
    }
    static int first_diff_bit(const char* s1, size_t l1, const char* s2, size_t l2) {
        size_t max_l = (l1 > l2) ? l1 : l2;
        for (size_t i = 0; i < max_l; ++i) {
            char c1 = (i < l1) ? s1[i] : 0;
            char c2 = (i < l2) ? s2[i] : 0;
            if (c1 != c2) {
                char diff = c1 ^ c2;
                for (int b = 0; b < 8; ++b) if ((diff >> b) & 1) return (int)(i * 8 + b);
            }
        }
        return -1;
    }
};

template <class T>
class PatriciaTree {
private:
    struct Node {
        Node *left = nullptr, *right = nullptr, *parent = nullptr;
        String key;
        T value = 0;
        int diffBit = -1;
    };
    Node* root = nullptr;
    size_t count = 0;

    void to_lower(const char* src, char* dst) const {
        size_t i = 0;
        for (; src[i]; ++i) dst[i] = (src[i] >= 'A' && src[i] <= 'Z') ? (src[i] + 32) : src[i];
        dst[i] = '\0';
    }

    Node* find_closest(const char* key, size_t len) const {
        if (!root) return nullptr;
        Node* curr = root;
        while (curr->diffBit != -1) {
            if (LatinDigitizer::get_bit(key, len, curr->diffBit)) curr = curr->right;
            else curr = curr->left;
        }
        return curr;
    }

public:
    PatriciaTree() = default;
    ~PatriciaTree() { clear(); }

    void clear() {
        while (root) {
            Node* leaf = root;
            while (leaf->diffBit != -1) leaf = leaf->left;
            erase_node(leaf);
        }
    }

    bool find(const char* key_orig, T& result) const {
        char buf[512]; to_lower(key_orig, buf);
        size_t len = strlen(buf);
        Node* closest = find_closest(buf, len);
        if (closest && strcmp(closest->key.c_str(), buf) == 0) {
            result = closest->value;
            return true;
        }
        return false;
    }

    bool insert(const char* key_orig, T val) {
        char buf[512]; to_lower(key_orig, buf);
        size_t len = strlen(buf);
        if (!root) {
            root = new Node{nullptr, nullptr, nullptr, buf, val, -1};
            count = 1; return true;
        }
        Node* closest = find_closest(buf, len);
        int dbit = LatinDigitizer::first_diff_bit(buf, len, closest->key.c_str(), closest->key.size());
        if (dbit == -1) return false;
        Node* newNode = new Node{nullptr, nullptr, nullptr, buf, val, -1};
        Node* newInt = new Node{nullptr, nullptr, nullptr, "", 0, dbit};
        Node* curr = root, *par = nullptr;
        while (curr->diffBit != -1 && curr->diffBit < dbit) {
            par = curr;
            if (LatinDigitizer::get_bit(buf, len, curr->diffBit)) curr = curr->right;
            else curr = curr->left;
        }
        if (LatinDigitizer::get_bit(buf, len, dbit)) { newInt->left = curr; newInt->right = newNode; }
        else { newInt->left = newNode; newInt->right = curr; }
        newNode->parent = newInt; curr->parent = newInt; newInt->parent = par;
        if (!par) root = newInt;
        else if (par->left == curr) par->left = newInt;
        else par->right = newInt;
        count++; return true;
    }

    void erase_node(Node* leaf) {
        Node* internal = leaf->parent;
        if (!internal) { delete leaf; root = nullptr; }
        else {
            Node* sib = (internal->left == leaf) ? internal->right : internal->left;
            Node* grand = internal->parent;
            sib->parent = grand;
            if (!grand) root = sib;
            else if (grand->left == internal) grand->left = sib;
            else grand->right = sib;
            delete leaf; delete internal;
        }
        count--;
    }

    bool erase(const char* key_orig) {
        char buf[512]; to_lower(key_orig, buf);
        Node* closest = find_closest(buf, strlen(buf));
        if (!closest || strcmp(closest->key.c_str(), buf) != 0) return false;
        erase_node(closest); return true;
    }

    bool save(const char* path) const {
        std::ofstream os(path, std::ios::binary);
        if (!os) return false;
        os.write((char*)&count, sizeof(count));
        if (!root) return os.good();
        Node* stack[1024]; int top = 0; stack[top++] = root;
        while (top > 0) {
            Node* n = stack[--top];
            bool isL = (n->diffBit == -1);
            os.write((char*)&isL, sizeof(isL));
            if (isL) {
                size_t l = n->key.size();
                os.write((char*)&l, sizeof(l));
                os.write(n->key.c_str(), l);
                os.write((char*)&n->value, sizeof(T));
            } else {
                os.write((char*)&n->diffBit, sizeof(n->diffBit));
                stack[top++] = n->right; stack[top++] = n->left;
            }
        }
        return os.good();
    }

    bool load(const char* path) {
        std::ifstream is(path, std::ios::binary);
        if (!is) return false;
        size_t new_c; if (!is.read((char*)&new_c, sizeof(new_c))) return false;
        clear(); if (new_c == 0) return true;

        struct Frame { Node* n; bool leftDone; };
        Frame stack[1024]; int top = 0;

        auto read_one = [&]() -> Node* {
            bool isL; if (!is.read((char*)&isL, sizeof(isL))) return nullptr;
            Node* n = new Node();
            if (isL) {
                size_t l; is.read((char*)&l, sizeof(l));
                char b[512]; is.read(b, l); b[l] = '\0';
                n->key = b; is.read((char*)&n->value, sizeof(T)); n->diffBit = -1;
            } else is.read((char*)&n->diffBit, sizeof(n->diffBit));
            return n;
        };

        root = read_one();
        if (root->diffBit != -1) stack[top++] = {root, false};
        while (top > 0) {
            Node* next = read_one();
            if (!next) break; 
            Frame& curr = stack[top - 1];
            if (!curr.leftDone) {
                curr.n->left = next; next->parent = curr.n; curr.leftDone = true;
            } else {
                curr.n->right = next; next->parent = curr.n; top--;
            }
            if (next->diffBit != -1) stack[top++] = {next, false};
        }
        count = new_c; return true;
    }
};

int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    PatriciaTree<uint64_t> tree; char cmd[512];
    while (std::cin >> cmd) {
        if (cmd[0] == '+') {
            char w[512]; uint64_t v; std::cin >> w >> v;
            if (tree.insert(w, v)) std::cout << "OK\n"; else std::cout << "Exist\n";
        } else if (cmd[0] == '-') {
            char w[512]; std::cin >> w;
            if (tree.erase(w)) std::cout << "OK\n"; else std::cout << "NoSuchWord\n";
        } else if (cmd[0] == '!') {
            char act[10], p[512]; std::cin >> act >> p;
            if (act[0] == 'S') { if (tree.save(p)) std::cout << "OK\n"; else std::cout << "ERROR: save\n"; }
            else { if (tree.load(p)) std::cout << "OK\n"; else std::cout << "ERROR: load\n"; }
        } else {
            uint64_t r; if (tree.find(cmd, r)) std::cout << "OK: " << r << "\n";
            else std::cout << "NoSuchWord\n";
        }
    }
    return 0;
}