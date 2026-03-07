#include <vector>



using namespace std;



class Solution{
public:
    int hIndex(vector<int>& citations) {
        int n = citations.size();
        vector<int> buckets(n + 1,0);

        for(int citation : citations){
            if(citation >= n){
                buckets[n]++;
            } else {
                buckets[citation]++;
            }
        }
 
        int stats = 0;
        for(int h = n;h >= 0;h--){
            stats += buckets[h];
            if(stats >= h){
                return h;
            }
        }
        return 0;
    }
};



 

