


template <typename T>
class BinaryTree {
private:
    struct Node{
        T *data;
        Node *left;
        Node *right;
        Node(const &T val) : data(val), left(nullptr), right(nullptr) {}
    };

    Node *root;
    size_t size;
    size_t h;
    
