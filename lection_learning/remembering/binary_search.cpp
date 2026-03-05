#include <iostream>
#include <vector>


int binary_search(std::vector<int> &v, int target){
    int left = 0;
    int right = v.size() - 1;

    while(left <= right){
        int mid = (left + right) / 2;
        if (v[mid] == target){
            return v[mid];
            std::cout << v[mid]  << "v[mid] == target "<< '\n';
        } else if(v[mid] < target){
            left = mid + 1;
            std::cout << v[mid] << " v[mid] < target "<< '\n';
        } else {
            right = mid - 1;
            std::cout << v[mid] << " v[mid] > target" << '\n';
        }
    }
    return -1;
}


int main(){
    std::vector<int> v = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    int target = 20;
    std::cout << binary_search(v,target) << " ";
    return 0;
}