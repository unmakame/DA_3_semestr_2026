#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

class SuffixTree final {
public:
    struct Node {
        unordered_map<unsigned char, Node*> edges;
        Node* link;
        int start, end;
        Node() : link(nullptr), start(0), end(0) {}
    };

private:
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
        if (child->end == -1) return leafEnd - child->start;
        return child->end - child->start;
    }

    void extend() {
        Node* lastParent = nullptr;
        while (remainder > 0) {
            if (activeLen == 0) activeEdge = (unsigned char)text[endPos];
            unsigned char ch = (unsigned char)activeEdge;
            auto it = activeNode->edges.find(ch);
            if (it == activeNode->edges.end()) {
                Node* leaf = new Node();
                leaf->start = endPos;
                leaf->end = -1;
                activeNode->edges[ch] = leaf;
                if (lastParent) lastParent->link = activeNode;
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
                    if (lastParent) lastParent->link = activeNode;
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
                leaf->end = -1;
                split->edges[(unsigned char)text[endPos]] = leaf;
                split->edges[(unsigned char)text[child->start + activeLen]] = child;
                child->start += activeLen;
                activeNode->edges[ch] = split;
                if (lastParent) lastParent->link = split;
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
        if (lastParent) lastParent->link = root;
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

    ~SuffixTree() { deleteTree(root); }

    void deleteTree(Node* node) {
        if (!node) return;
        for (auto& p : node->edges) deleteTree(p.second);
        delete node;
    }

    const string& getText() const { return text; }

    int findMinimalRotationPosition(int n) const {
        Node* current = root;
        int depth = 0;
        int pos = 0;
        while (depth < n && !current->edges.empty()) {
            unsigned char minChar = 255;
            Node* minNode = nullptr;
            for (auto& p : current->edges) {
                if (p.first != '$' && p.first < minChar) {
                    minChar = p.first;
                    minNode = p.second;
                }
            }
            if (!minNode) break;
            int edgeLen;
            if (minNode->end == -1) edgeLen = leafEnd - minNode->start;
            else edgeLen = minNode->end - minNode->start;
            int take = min(edgeLen, n - depth);
            depth += take;
            pos = minNode->start + take - 1;
            if (take == edgeLen) current = minNode;
            else break;
        }
        return (pos - n + 1 + 2 * n) % n;
    }
};

int main() {
    string s;
    cin >> s;
    int n = s.size();
    string doubled = s + s;
    SuffixTree tree(doubled);
    int minPos = tree.findMinimalRotationPosition(n);
    cout << doubled.substr(minPos, n) << endl;
    return 0;
}