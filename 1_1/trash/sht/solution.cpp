#include <unordered_map>
#include <iostream>
#include <string>


using namespace std;

class SuffixTree final {
private:
    struct Node {
        unordered_map<unsigned char, Node*> edges; 
        Node* link;
        int start, end;
        
        Node() : link(nullptr), start(0), end(0) {}
    };

    string text;
    Node* root;
    Node* activeNode;
    int activeEdge;
    int activeLen;
    int endPos;
    int remainder;
    int leafEnd;

    int getEdgeLen(Node* node, unsigned char ch) const {
        auto it = node->edges.find(ch);
        if (it == node->edges.end()) return 0;
        Node* child = it->second;
        if (child->end > endPos) {
            return endPos - child->start + 1;
        }
        return child->end - child->start;
    }

    void extend() {
        Node* lastParent = nullptr;
        
        while (remainder > 0) {
            if (activeLen == 0) {
                activeEdge = (unsigned char)text[endPos];
            }

            unsigned char ch = (unsigned char)activeEdge;
            auto it = activeNode->edges.find(ch);
            
            if (it == activeNode->edges.end()) {
                Node* leaf = new Node();
                leaf->start = endPos;
                leaf->end = leafEnd;
                activeNode->edges[ch] = leaf;
                
                if (lastParent) {
                    lastParent->link = activeNode;
                }
                lastParent = activeNode;
            } else {
                Node* child = it->second;
                int edgeLen = getEdgeLen(activeNode, ch);
                
                if (activeLen >= edgeLen) {
                    activeEdge = (unsigned char)text[endPos - remainder + 1 + edgeLen];
                    activeLen -= edgeLen;
                    activeNode = child;
                    continue;
                }

                if (text[child->start + activeLen] == text[endPos]) {
                    if (lastParent) {
                        lastParent->link = activeNode;
                    }
                    lastParent = nullptr;
                    activeLen++;
                    return;
                }

                Node* split = new Node();
                split->start = child->start;
                split->end = child->start + activeLen;
                split->link = root;
                
                Node* leaf = new Node();
                leaf->start = endPos;
                leaf->end = leafEnd;
                split->edges[(unsigned char)text[endPos]] = leaf;
                split->edges[(unsigned char)text[child->start + activeLen]] = child;
                
                child->start += activeLen;
                activeNode->edges[ch] = split;
                
                if (lastParent) {
                    lastParent->link = split;
                }
                lastParent = split;
            }

            remainder--;
            
            if (activeNode == root && activeLen > 0) {
                activeLen--;
                activeEdge = (unsigned char)text[endPos - remainder + 1];
            } else {
                activeNode = (activeNode->link) ? activeNode->link : root;
            }
        }

        if (lastParent) {
            lastParent->link = root;
        }
    }

public:
    SuffixTree(const string& s) 
        : text(s + "$"), root(nullptr), activeNode(nullptr), 
          activeEdge(0), activeLen(0), endPos(0), remainder(0), leafEnd(0) {
        
        root = new Node();
        root->link = root;
        activeNode = root;
        leafEnd = 0;

        for (size_t i = 0; i < text.length(); ++i) {
            endPos = i;
            remainder++;
            leafEnd = i + 1;
            extend();
        }
    }

    ~SuffixTree() {
        deleteTree(root);
    }

    void deleteTree(Node* node) {
        if (!node) return;
        for (auto& p : node->edges) {
            deleteTree(p.second);
        }
        delete node;
    }

    string getSuffix(size_t pos) {
        if (pos >= text.length() - 1) return "";
        return text.substr(pos);
    }

    const string& getText() const { return text; }
    size_t getTextLength() const { return text.length(); }
};

string findMinimalRotation(string s) {
    int n = s.length();
    string doubled = s + s;
    SuffixTree tree(doubled);
    const string& text = tree.getText();
    
    int minPos = 0;
    
    for (int i = 1; i < n; ++i) {
        bool needSwap = false;
        
        for (int j = 0; j < n; ++j) {
            char c1 = text[minPos + j];
            char c2 = text[i + j];
            
            if (c2 < c1) {
                needSwap = true;
                break;
            } else if (c1 < c2) {
                break;
            }
        }
        
        if (needSwap) {
            minPos = i;
        }
    }
    
    return text.substr(minPos, n);
}

int main() {
    string input;
    getline(cin, input);
    string result = findMinimalRotation(input);
    cout << result << endl;
    return 0;
}
