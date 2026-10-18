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

    void insert(const T& val, Node *&node, Node *parent){
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

    bool delete_node(const T& val, Node*& node) {  
    if (node == nullptr) return false;
    
    if (val < node->data) {
        return delete_node(val, node->left);
    }
    else if (val > node->data) {
        return delete_node(val, node->right);
    }
    else {  
        Node* toDelete = node;
        
        if (node->left == nullptr && node->right == nullptr) {
            node = nullptr;
            delete toDelete;
            size--;
            return true;
        }
        else if (node->left == nullptr) {
            node = node->right;
            if (node != nullptr) {
                node->parent = toDelete->parent;
            }
            delete toDelete;
            size--;
            return true;
        }
        else if (node->right == nullptr) {
            node = node->left;
            if (node != nullptr) {
                node->parent = toDelete->parent;
            }
            delete toDelete;
            size--;
            return true;
        }
        else {
            Node* parent = node;
            Node* successor = node->right;
            
            while (successor->left != nullptr) {
                parent = successor;
                successor = successor->left;
            }
            
            node->data = successor->data;
            
            if (successor->right != nullptr) {
                successor->right->parent = parent;
            }
            
            if (parent == node) {
                parent->right = successor->right;
            } else {
                parent->left = successor->right;
            }
            
            delete successor;
            size--;
            return true;
        }
    }
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
        return search(val,root);
    }

};


int main(){
    BinaryTree<int> tree;
    tree.insert(5);
    return 0;
}
