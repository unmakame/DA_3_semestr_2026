#include <iostream>
#include <vector>



void insertion_sort(std::vector<int> &v){
    for(int i = 1;i < v.size();i++){
        int key = v[i];
        int j = i - 1;

        while(j >= 0 && v[j] > key){
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = key;
    }
}

void print_v(const std::vector<int> &v){
    for(int i = 0; i < v.size(); i++){
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
}

int main(){
    std::vector<int> numbers = {100000,111,10};
    print_v(numbers);
    insertion_sort(numbers);
    print_v(numbers);
}