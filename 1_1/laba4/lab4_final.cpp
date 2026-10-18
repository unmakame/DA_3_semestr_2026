#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <cstdint>

using uint = uint32_t;

std::vector<uint> parse_line(const std::string& line) {
    std::vector<uint> v;
    std::stringstream ss(line);
    uint x;
    while (ss >> x) v.push_back(x);
    return v;
}

std::unordered_map<uint,int> build_bad_char(const std::vector<uint>& pat) {
    std::unordered_map<uint,int> bad;
    for (int i = 0; i < (int)pat.size(); i++)
        bad[pat[i]] = i;
    return bad;
}

std::vector<int> build_good_suffix(const std::vector<uint>& pat) {
    int m = (int)pat.size();

    std::vector<int> shift(m + 1, 0);
    std::vector<int> border(m + 1, 0);

    int i = m;
    int j = m + 1;
    border[i] = j;

    while (i > 0) {
        while (j <= m && pat[i - 1] != pat[j - 1]) {
            if (shift[j] == 0)
                shift[j] = j - i;
            j = border[j];
        }
        --i;
        --j;
        border[i] = j;
    }

    j = border[0];

    for (i = 0; i <= m; i++) {
        if (shift[i] == 0)
            shift[i] = j;
        if (i == j)
            j = border[j];
    }

    return shift;
}

// БЕЗ SKIP ARRAY - ПРОСТАЯ И ПРАВИЛЬНАЯ ВЕРСИЯ
void search_buffer(
    const std::vector<uint>& text,
    const std::vector<std::pair<int,int>>& pos,
    const std::vector<uint>& pat,
    const std::unordered_map<uint,int>& bad,
    const std::vector<int>& good
) {
    int n = (int)text.size();
    int m = (int)pat.size();

    int s = 0;

    while (s <= n - m) {
        int j = m - 1;

        while (j >= 0 && pat[j] == text[s + j])
            --j;

        if (j < 0) {
            // ПРОВЕРКА: совпадение только внутри одной строки!
            if (pos[s].first == pos[s + m - 1].first) {
                std::cout << pos[s].first << ", " << pos[s].second << '\n';
            }
            s += good[0];
        } else {
            int bc;
            auto it = bad.find(text[s + j]);

            if (it != bad.end())
                bc = std::max(1, j - it->second);
            else
                bc = j + 1;

            int gs = good[j + 1];
            s += std::max(bc, gs);
        }
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string line;

    if (!std::getline(std::cin, line))
        return 0;

    std::vector<uint> pattern = parse_line(line);

    if (pattern.empty())
        return 0;

    int m = (int)pattern.size();

    auto bad = build_bad_char(pattern);
    auto good = build_good_suffix(pattern);

    std::vector<uint> buffer;
    std::vector<std::pair<int,int>> pos;

    buffer.reserve(m);
    pos.reserve(m);

    int line_num = 1;

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;  // Пропускаем пустые строки
        
        std::stringstream ss(line);

        uint x;
        int word_num = 1;

        while (ss >> x) {
            buffer.push_back(x);
            pos.push_back({line_num, word_num});

            if ((int)buffer.size() == m) {
                search_buffer(buffer, pos, pattern, bad, good);

                buffer.erase(buffer.begin());
                pos.erase(pos.begin());
            }

            word_num++;
        }

        line_num++;
    }

    return 0;
}
