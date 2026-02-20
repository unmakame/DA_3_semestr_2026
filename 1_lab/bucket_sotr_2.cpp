#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <cstdint>
#include <limits>

using u = std::uint64_t;
using el = std::pair<u,std::string>;

u parse_u64(const std::string& s){
    u result = 0;
    for(char c : s){
        result = result * 10 + (c - '0');
    }
    return result;
}

void insertion_sort(std::vector<el> &buck){
    for(size_t i = 1; i < buck.size(); i++){
        u key = buck[i].first;
        std::string val = std::move(buck[i].second);
        size_t left = 0;
        size_t right = i;
        while(left < right){
            size_t mid = left + (right - left) / 2;
            if(buck[mid].first <= key){
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        for(size_t j = i; j > left; j--){
            buck[j] = std::move(buck[j - 1]);
        }
        buck[left] = {key, std::move(val)};
    }
}

void bucket_sort(std::vector<std::vector<el>>& buck){
    std::string line;
    std::vector<el> temp_data;
    temp_data.reserve(1 << 20);
    u min_val = std::numeric_limits<u>::max();
    u max_val = std::numeric_limits<u>::min();
    while(std::getline(std::cin, line)){
        if(line.empty()) continue;
        size_t tab = line.find('\t');
        if(tab == std::string::npos) continue;
        u key = parse_u64(line.substr(0, tab));
        std::string value = line.substr(tab + 1);
        temp_data.emplace_back(key, std::move(value));
        if(key < min_val) min_val = key;
        if(key > max_val) max_val = key;
    }

    if(temp_data.empty()) return;
    u range = max_val - min_val;
    for(auto& b : buck){
        b.reserve(temp_data.size() / buck.size() + 1);
    }

    auto get_ind = [&](u key)->size_t{
        if(range == 0) return 0;
        return (key - min_val) * (buck.size() - 1) / range;
    };

    for(auto& item : temp_data){
        size_t idx = get_ind(item.first);
        buck[idx].emplace_back(std::move(item));
    }
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::vector<std::vector<el>> buckets(65);
    bucket_sort(buckets);
    for(auto& b : buckets){
        if(b.size() > 1){
            insertion_sort(b);
        }
    }

    for(const auto& b : buckets){
        for(const auto& p : b){
            std::cout << p.first << "\t" << p.second << '\n';
        }
    }
    return 0;
}