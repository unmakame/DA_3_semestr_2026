#include <vector>
// #include <iostream>

using namespace std;
class Solution{
public:
    int hIndex(vector<int>& citations) {
        int n = citations.size();
        int max_citation = 0;

        for(int citation : citations){
            if (citation >= max_citation){
                max_citation = citation;
            }
        }

        int h_indx = 0;

        for(int h = 1; h <= max_citation; h++){
            int count = 0;
            for(int citation : citations){
                if(citation >= h){
                    count++;
                }
            }
            if(count >= h){
                h_indx = h;
            }
        }
        return h_indx;
    }
};