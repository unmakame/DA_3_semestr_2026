#include <iostream>
#include <vector>
#include <algorithm>


void count_rad(std::vector<int> &v,int exp){
    const int n = v.size();
    std::vector<int> output(n);
    std::vector<int> count(10,0);

    for(int i = 0; i < n ;i++){
        int digit = (v[i] / exp) % 10;
        count[digit]++;
    }

    for(int i = 1; i < 10; i++){
        count[i] += count[i - 1];
    }

    for(int i = n - 1; i >= 0; i--){
        int digit = (v[i] / exp) % 10;
        output[count[digit] - 1] = v[i];
        count[digit]--;
    }

    for(int i = 0;i < n; i++){
        v[i] = output[i];
    }
}

void rad_sort(std::vector<int> &v){
    if(v.empty()){
        return;
    }
    int max = *std::max_element(v.begin(),v.end());
    for(int exp = 1; max / exp > 0;exp*=10){
        count_rad(v,exp);
    }
}

void print_v(const std::vector<int> &v){
    for(int i = 0; i < v.size(); i++){
        std::cout << v[i] << " ";
    }
}

int main(){
    std::vector<int> v = {1,12,4,23,5,23,67,52,42};
    print_v(v);
    std::cout << " ";
    rad_sort(v);
    std::cout << " " << std::endl;
    print_v(v);
}