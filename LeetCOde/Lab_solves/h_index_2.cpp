#include <vector>



using namespace std;



class Solution{
public:
    int hIndex(vector<int>& citations) {
        int n = citations.size();
        vector<int> buck(n + 1, 0);

        for(int citation : citations){
            if(citations[citation] >= n){
                buck[n]++;
            } else {
                buck[citation]++;
            }
        }

        int stats = 0;
        for(int h = n; h >= 0; h--){
            stats+=buck[h];
            if(stats>=h){
                return h;
            }
        }
};



 

