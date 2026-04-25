#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <sstream>
#include <fstream>

using namespace std;

typedef unsigned int uint;

class ApostolicoGiancarlo {
private:
    vector<uint> pattern;
    vector<int> failure;
    
public:
    ApostolicoGiancarlo(const vector<uint>& p) : pattern(p) {
        build_failure_function();
    }
    
    void build_failure_function() {
        int m = pattern.size();
        failure.assign(m, 0);
        
        for (int i = 1; i < m; i++) {
            int j = failure[i - 1];
            while (j > 0 && pattern[i] != pattern[j]) {
                j = failure[j - 1];
            }
            if (pattern[i] == pattern[j]) {
                j++;
            }
            failure[i] = j;
        }
    }
    
    vector<int> search(const vector<uint>& text) {
        vector<int> matches;
        int n = text.size();
        int m = pattern.size();
        
        if (m == 0 || n < m) return matches;
        
        map<uint, int> bad_char;
        for (int i = 0; i < m - 1; ++i) {
            bad_char[pattern[i]] = i;
        }
        
        int i = 0;
        while (i <= n - m) {
            int j;
            for (j = m - 1; j >= 0 && pattern[j] == text[i + j]; --j) {}
            
            if (j < 0) {
                matches.push_back(i);
                i++;
            } else {
                int bad_char_shift = j;
                if (bad_char.count(text[i + j])) {
                    bad_char_shift = j - bad_char[text[i + j]];
                } else {
                    bad_char_shift = j + 1;
                }
                
                int kmp_shift = 1;
                if (j > 0) {
                    kmp_shift = j - failure[j - 1];
                }
                
                i += max(bad_char_shift, kmp_shift);
            }
        }
        
        return matches;
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    ifstream input_file("input.txt");
    ofstream output_file("output.txt");
    
    istream* input = input_file.is_open() ? (istream*)&input_file : &cin;
    ostream* output = output_file.is_open() ? (ostream*)&output_file : &cout;
    
    vector<uint> pattern;
    string line;
    
    while (getline(*input, line)) {
        if (line.empty()) break;
        
        stringstream ss(line);
        uint num;
        while (ss >> num) {
            pattern.push_back(num);
        }
    }
    
    if (pattern.empty()) {
        return 0;
    }
    
    ApostolicoGiancarlo ag(pattern);
    
    int line_num = 1;
    while (getline(*input, line)) {
        vector<uint> text_line;
        stringstream ss(line);
        uint num;
        
        while (ss >> num) {
            text_line.push_back(num);
        }
        
        if (!text_line.empty()) {
            vector<int> matches = ag.search(text_line);
            
            for (int match_pos : matches) {
                int word_pos = match_pos + 1;
                *output << line_num << ", " << word_pos << "\n";
            }
        }
        
        line_num++;
    }
    
    return 0;
}
