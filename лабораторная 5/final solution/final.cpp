#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

const int INF = 1e9;

struct Node {
    int start;
    int* end;
    map<char, int> ch;
    int suffix_link;

    Node(int start, int* end) : start(start), end(end), suffix_link(0) {}
};

class SuffixTree {
private:
    string t;
    vector<Node> tree;
    int root;
    
    int active_node;
    int active_edge;
    int active_length;
    int remaining;
    int leaf_end;

    void extend(int pos) {
        leaf_end = pos;
        remaining++;
        int last_created_internal_node = 0;

        while (remaining > 0) {
            if (active_length == 0) {
                active_edge = pos;
            }

            char current_char = t[active_edge];
            
            if (tree[active_node].ch.find(current_char) == tree[active_node].ch.end()) {
                tree.push_back(Node(pos, &leaf_end));
                tree[active_node].ch[current_char] = tree.size() - 1;

                if (last_created_internal_node > 0) {
                    tree[last_created_internal_node].suffix_link = active_node;
                    last_created_internal_node = 0;
                }
            } else {
                int next_node = tree[active_node].ch[current_char];
                int edge_length = *(tree[next_node].end) - tree[next_node].start + 1;

                if (active_length >= edge_length) {
                    active_edge += edge_length;
                    active_length -= edge_length;
                    active_node = next_node;
                    continue;
                }

                if (t[tree[next_node].start + active_length] == t[pos]) {
                    if (last_created_internal_node > 0 && active_node != root) {
                        tree[last_created_internal_node].suffix_link = active_node;
                    }
                    active_length++;
                    break;
                }

                int* split_end = new int(tree[next_node].start + active_length - 1);
                tree.push_back(Node(tree[next_node].start, split_end));
                int split_node = tree.size() - 1;

                tree[active_node].ch[current_char] = split_node;

                tree.push_back(Node(pos, &leaf_end));
                tree[split_node].ch[t[pos]] = tree.size() - 1;

                tree[next_node].start += active_length;
                tree[split_node].ch[t[tree[next_node].start]] = next_node;

                if (last_created_internal_node > 0) {
                    tree[last_created_internal_node].suffix_link = split_node;
                }
                last_created_internal_node = split_node;
            }

            remaining--;
            if (active_node == root && active_length > 0) {
                active_length--;
                active_edge = pos - remaining + 1;
            } else if (active_node != root) {
                active_node = tree[active_node].suffix_link;
            }
        }
    }

public:
    SuffixTree(const string& str) {
        t = str;
        root = 0;
        int* root_end = new int(-1);
        tree.push_back(Node(-1, root_end));
        
        active_node = root;
        active_edge = -1;
        active_length = 0;
        remaining = 0;
        leaf_end = -1;

        for (int i = 0; i < (int)t.length(); ++i) {
            extend(i);
        }
    }

    string find_min_cut(int n) {
        string result = "";
        int current_node = root;
        int needed_length = n;

        while (needed_length > 0) {
            auto it = tree[current_node].ch.begin();
            
            if (it->first == '$' && needed_length > 0 && tree[current_node].ch.size() > 1) {
                it++;
            }

            int next_node = it->second;
            int start = tree[next_node].start;
            int end = *(tree[next_node].end);
            int edge_len = end - start + 1;

            int chars_to_take = min(needed_length, edge_len);
            result += t.substr(start, chars_to_take);

            needed_length -= chars_to_take;
            current_node = next_node;
        }
        return result;
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    string s;
    if (cin >> s) {
        int n = s.length();
        string t = s + s + "$";
        
        SuffixTree st(t);
        cout << st.find_min_cut(n) << "\n";
    }

    return 0;
}