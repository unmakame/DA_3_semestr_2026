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

bool read_pattern(std::vector<uint>& pattern) {
    std::string line;

    if (!std::getline(std::cin, line))
        return false;

    pattern = parse_line(line);
    return !pattern.empty();
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

void print_match(const std::pair<int,int>& p) {
    std::cout << p.first << ", " << p.second << '\n';
}

void apostolico_giancarlo(
    const std::vector<uint>& text,
    const std::vector<std::pair<int,int>>& pos,
    const std::vector<uint>& pattern,
    const std::unordered_map<uint,int>& bad,
    const std::vector<int>& good
) {
    int n = (int)text.size();
    int m = (int)pattern.size();

    if (n < m)
        return;

    std::vector<int> skip(n, 0);

    int s = 0;

    while (s <= n - m) {
        int j = m - 1;

        while (j >= 0) {
            int t = s + j;

            if (skip[t] > 0) {
                int k = skip[t];

                if (k > j || k == j + 1) {
                    j = -1;
                    break;
                }

                j -= k;
                continue;
            }

            if (pattern[j] == text[t]) {
                --j;
            } else {
                break;
            }
        }

        if (j < 0) {
            // ✓ ИСПРАВКА #1: Проверяем что ВСЕ элементы совпадения в одной строке
            if (pos[s].first == pos[s + m - 1].first) {
                print_match(pos[s]);
            }

            skip[s + m - 1] = m;
            s += good[0];
        } else {
            int matched = m - 1 - j;

            if (matched > 0)
                skip[s + m - 1] = matched;

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

void process_text(
    const std::vector<uint>& pattern,
    const std::unordered_map<uint,int>& bad,
    const std::vector<int>& good
) {
    int m = (int)pattern.size();

    std::vector<uint> buffer;
    std::vector<std::pair<int,int>> pos;

    buffer.reserve(m);
    pos.reserve(m);

    std::string line;
    int line_num = 1;

    while (std::getline(std::cin, line)) {
        // ✓ ИСПРАВКА #2: Пропускаем пустые строки
        if (line.empty()) continue;
        
        std::stringstream ss(line);

        uint x;
        int word_num = 1;

        while (ss >> x) {
            buffer.push_back(x);
            pos.push_back({line_num, word_num});

            if ((int)buffer.size() == m) {
                apostolico_giancarlo(buffer, pos, pattern, bad, good);

                buffer.erase(buffer.begin());
                pos.erase(pos.begin());
            }

            word_num++;
        }

        line_num++;
    }
}

void solve() {
    std::vector<uint> pattern;

    if (!read_pattern(pattern))
        return;

    auto bad = build_bad_char(pattern);
    auto good = build_good_suffix(pattern);

    process_text(pattern, bad, good);
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    solve();
    return 0;
}
