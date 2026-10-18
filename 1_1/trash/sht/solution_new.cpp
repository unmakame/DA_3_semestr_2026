#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <functional>

using namespace std;

class SuffixTree {
public:
    struct Node {
        unordered_map<unsigned char, Node*> edges;
        Node* link;
        int start, end;
        
        Node(int s = 0, int e = 0) : link(nullptr), start(s), end(e) {}
    };

private:
    string text;
    Node* root;
    Node* activeNode;
    int activeEdge;
    int activeLen;
    int endPos;
    int remainder;
    
    int getEdgeLen(Node* child) const {
        if (child->end == -1) {
            return endPos - child->start + 1;
        }
        return child->end - child->start + 1;
    }

    void extend() {
        Node* lastParent = nullptr;
        
        while (remainder > 0) {
            if (activeLen == 0) {
                activeEdge = text[endPos];
            }
            
            auto it = activeNode->edges.find(activeEdge);
            
            if (it == activeNode->edges.end()) {
                Node* leaf = new Node(endPos - remainder + 1, -1);
                activeNode->edges[activeEdge] = leaf;
                
                if (lastParent) {
                    lastParent->link = activeNode;
                }
                lastParent = activeNode;
            } else {
                Node* child = it->second;
                int len = getEdgeLen(child);
                
                if (activeLen >= len) {
                    activeEdge = text[endPos - remainder + 1 + len];
                    activeLen -= len;
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
                
                Node* split = new Node(child->start, child->start + activeLen - 1);
                split->link = root;
                
                Node* leaf = new Node(endPos - remainder + 1, -1);
                split->edges[text[endPos]] = leaf;
                split->edges[text[child->start + activeLen]] = child;
                
                child->start += activeLen;
                activeNode->edges[activeEdge] = split;
                
                if (lastParent) {
                    lastParent->link = split;
                }
                lastParent = split;
            }
            
            remainder--;
            
            if (activeNode == root && activeLen > 0) {
                activeLen--;
                activeEdge = text[endPos - remainder + 1];
            } else {
                activeNode = activeNode->link ? activeNode->link : root;
            }
        }
        
        if (lastParent) {
            lastParent->link = root;
        }
    }

public:
    SuffixTree(const string& s) : text(s + "$"), root(nullptr), activeNode(nullptr),
                                   activeEdge(0), activeLen(0), endPos(0), remainder(0) {
        root = new Node();
        root->link = root;
        activeNode = root;
        
        for (int i = 0; i < (int)text.length(); i++) {
            endPos = i;
            remainder++;
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

    int findMinimalRotation(int rotationLen) const {
        int n = rotationLen;

        function<int(Node*)> find_leftmost = [&](Node* node) -> int {
            // collect child keys and sort
            vector<unsigned char> keys;
            keys.reserve(node->edges.size());
            for (auto &p : node->edges) keys.push_back(p.first);
            sort(keys.begin(), keys.end());
            for (unsigned char k : keys) {
                if (k == '$') continue;
                Node* child = node->edges.at(k);
                if (child->edges.empty()) {
                    if (child->start < n) return child->start;
                    else continue;
                }
                int res = find_leftmost(child);
                if (res != -1) return res;
            }
            return -1;
        };

        // collect and print root keys
        cerr << "root keys: ";
        vector<unsigned char> rootkeys;
        for (auto &p : root->edges) rootkeys.push_back(p.first);
        sort(rootkeys.begin(), rootkeys.end());
        for (auto k : rootkeys) cerr << k << ' ';
        cerr << '\n';

        // print leftmost leaf start for each root child
        for (auto k : rootkeys) {
            if (k == '$') continue;
            int r = find_leftmost(root->edges.at(k));
            cerr << "child " << k << " -> " << r << '\n';
        }

        // collect all leaf starts
        vector<int> leaves;
        function<void(Node*)> collect = [&](Node* node) {
            if (node->edges.empty()) {
                leaves.push_back(node->start);
                return;
            }
            for (auto &p : node->edges) collect(p.second);
        };
        collect(root);
        cerr << "all leaves: ";
        sort(leaves.begin(), leaves.end());
        for (int x : leaves) cerr << x << ' ';
        cerr << '\n';

            // Fallback: simple brute-force compare of all rotations using text
            int best = 0;
            for (int i = 1; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    unsigned char a = (unsigned char)text[i + j];
                    unsigned char b = (unsigned char)text[best + j];
                    if (a < b) { best = i; break; }
                    if (a > b) break;
                }
            }
            return best;
    }
};

string findMinimalRotation(string s) {
    int n = s.length();
    string doubled = s + s;
    
    SuffixTree tree(doubled);
    int pos = tree.findMinimalRotation(n);
    
    return doubled.substr(pos, n);
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    string input;
    getline(cin, input);
    
    string result = findMinimalRotation(input);
    cout << result << "\n";
    
    return 0;
}
