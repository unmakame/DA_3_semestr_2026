#include <iostream>
#include <vector>



using namespace std;
class Solution {
public:
    string longestCommonPrefix(vector<string>& strs) {
        if(strs.size() == 0) return "";
        string pref = "";
        for(size_t j = 0; j < strs[0].length();j++){
            char cur = strs[0][j];
            for(size_t i = 0; i < strs.size();i++){
                if(j >= strs[i].length() || strs[i][j] != cur){
                    return pref;
                }
            }
            pref += cur;
        }
        return pref;
    }
};;