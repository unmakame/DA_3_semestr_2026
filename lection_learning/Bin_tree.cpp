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
        }

        else if (node->left == nullptr) {
            node = node->right;
        }

        else if (node->right == nullptr) {
            node = node->left;
        }
   
        else {
            Node* parent = node;
            Node* successor = node->right;
            
           
            while (successor->left != nullptr) {
                parent = successor;
                successor = successor->left;
            }
            
          
            node->data = successor->data;
            
           
            if (parent == node) {
                parent->right = successor->right;
            } else {
                parent->left = successor->right;
            }
            
            delete successor;
            return true;  
        
        delete toDelete;  
        return true;
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
    tree.insert(3);
    tree.insert(7);
    tree.insert(1);
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(1);
    tree.insert(7);
    tree.insert(8);
    tree.insert(65);
    tree.insert(67);



    bool found = tree.search(67);
    std::cout << found << " ";
    return 0;
}
