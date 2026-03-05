#include <iostream>
#include <vector>




void bubble_sort(std::vector<size_t> &v){
    for(int i = 0; i < v.size() -1 ;i++){
        for(size_t j = 0; j < v.size() - 1 - i;j++){
            if(v[j] > v[j + 1]){
                int temp = v[j];
                v[j] = v[j + 1];
                v[j + 1] = temp;
            }
        }
    }
}


void print_v(std::vector<size_t> &v){
    for(int i = 0; i < v.size();i++){
        std::cout << v[i] << " ";
    }
}

int main(){
    std::vector<size_t> v = {10,1,9,4,2,4,11,25,423,21};
    print_v(v);
    std::cout << '\n';
    bubble_sort(v);
    print_v(v);
    return 0;
}