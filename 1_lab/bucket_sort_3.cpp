#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <cstdint>
#include <limits>
#include <algorithm>   

using u = std::uint64_t;
using el = std::pair<u, std::string>;

void bucket_sort(std::vector<std::vector<el>>& buck) {
    std::vector<el> temp_data;
    u min_val = std::numeric_limits<u>::max();
    u max_val = std::numeric_limits<u>::min();
    el item;  
    while (std::cin >> item.first >> item.second) {
        temp_data.push_back(std::move(item));  
        if (temp_data.back().first < min_val) min_val = temp_data.back().first;
        if (temp_data.back().first > max_val) max_val = temp_data.back().first;
    }
    
    if (temp_data.empty()) return;
    u range = max_val - min_val;
    buck.clear();
    buck.resize(temp_data.size());  
    
    auto get_ind = [&](u key) -> size_t {
        if (range == 0) return 0;
        double d = static_cast<double>(key - min_val) / range;
        return static_cast<size_t>(static_cast<double>(temp_data.size() - 1) * d);
    };
    
    for (auto& item : temp_data) {
        size_t idx = get_ind(item.first);
        buck[idx].emplace_back(std::move(item));
    }
}

void insertion_sort(std::vector<el>& buck) {
    for (size_t i = 1; i < buck.size(); i++) {
        u key = buck[i].first;
        std::string val = std::move(buck[i].second);
        size_t left = 0;
        size_t right = i;     
        while (left < right) {
            size_t mid = left + (right - left) / 2;
            if (buck[mid].first <= key) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        
        if (left < i) {
            std::move_backward(
                buck.begin() + left,
                buck.begin() + i,
                buck.begin() + i + 1
            );
        }
        buck[left] = {key, std::move(val)};
    }
}

int main() {
    std::vector<std::vector<el>> buckets;      
    bucket_sort(buckets);
    for (auto& b : buckets) {
        if (b.size() > 1) {
            insertion_sort(b);
        }
    }
    for (const auto& b : buckets) {
        for (const auto& p : b) {
            std::cout << p.first << "\t" << p.second << '\n';
        }
    }
    return 0;
}