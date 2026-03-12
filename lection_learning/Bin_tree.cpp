#include<iostream>


template <typename T>
class BinaryTree {
private:
    struct Node{
        T data;
        Node *parent;
        Node *left;
        Node *right;
        Node(const T& val) : data(val),parent(nullptr), left(nullptr), right(nullptr) {}
    };

    Node *root;
    size_t size;
    size_t h;

    void clear(Node *node){
        if(node == nullptr){
            return;
        } else {
            clear(node->left);
            clear(node->right);

            delete node;
        }
    }

    void insert(const T& val, Node *node, Node *parent){
        if(node == nullptr){
            node = new Node(val);
            node->parent = parent;
            return;
        } 

        if(node->data < val){
            insert(val,node->left,node);
        } 

        if(node->data > val){
            insert(val,node->right,node);
        }

    }

    bool search(const T& val, Node *node){
        if(node == nullptr){
            return false;
        }
        if(node->data == val){
            return true;
        }

        if(node->data < val){
            return search(val, node->left);
        } else {
            return search(val, node->right);
        }
        return false;
    }

public:
    BinaryTree() : root(nullptr), size(0), h(0) {}

    ~BinaryTree(){
        clear(root);
    }

    void insert(const T& val){
        insert(val,root,nullptr);
    }

    bool search(const T& val){
        search(val,root);
    }

};


int main(){
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(1);

    bool found = tree.search(3);
    return 0;
}
