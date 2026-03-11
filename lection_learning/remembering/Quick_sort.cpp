#include <iostream>
#include <vector>



void quick_sort(std::vector<size_t> &v,int left, int right){
    if(left >= right){
        return;
    }
    

    int pivot = v[(left + right) / 2];
    int i = left;
    int j = right;

    while( i <= j){
        while(v[i] < pivot){
            i++;
        }
        while(v[j] > pivot){
            j--;
        }
        if(i <= j){
            int temp = v[i];
            v[i] = v[j];
            v[j] = temp;
            i++;
            j--;
        }
    }
    quick_sort(v,left,j);
    quick_sort(v,i,right);
}


void print_v(std::vector<size_t> &v){
    for(int i = 0; i < v.size();i++){
        std::cout << v[i] << " ";
    }
}

int main(){
    std::vector<size_t> v = {1,4,2,5,3,2,9,6,10,23,21};
    print_v(v);
    std::cout << '\n';
    quick_sort(v,0,v.size() - 1);
    print_v(v);
    return 0;
}