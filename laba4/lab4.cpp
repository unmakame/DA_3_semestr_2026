#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <cstdint>

using uint = uint32_t;

struct InputData {
    std::vector<uint> pattern;
    std::vector<uint> text;
    std::vector<std::pair<int, int>> pos;
};

std::vector<uint> parse_uint_line(const std::string& line) {
    std::vector<uint> out;
    std::stringstream ss(line);
    uint x;
    while (ss >> x) out.push_back(x);
    return out;
}

std::unordered_map<uint, int> build_bad_char(const std::vector<uint>& pat) {
    std::unordered_map<uint, int> bad;
    for (int i = 0; i < static_cast<int>(pat.size()); ++i) {
        bad[pat[i]] = i;
    }
    return bad;
}

std::vector<int> build_good_suffix(const std::vector<uint>& pat) {
    int m = static_cast<int>(pat.size());
    std::vector<int> shift(m + 1, 0), border(m + 1, 0);

    int i = m, j = m + 1;
    border[i] = j;

    while (i > 0) {
        while (j <= m && pat[i - 1] != pat[j - 1]) {
            if (shift[j] == 0) shift[j] = j - i;
            j = border[j];
        }
        --i;
        --j;
        border[i] = j;
    }

    j = border[0];
    for (i = 0; i <= m; ++i) {
        if (shift[i] == 0) shift[i] = j;
        if (i == j) j = border[j];
    }
    return shift;
}

bool read_input(std::istream& in, InputData& data) {
    std::string line;

    if (!std::getline(in, line)) return false;
    data.pattern = parse_uint_line(line);
    if (data.pattern.empty()) return false;

    int line_num = 0;
    while (std::getline(in, line)) {
        ++line_num;
        std::stringstream ss(line);
        uint x;
        int word_num = 0;
        while (ss >> x) {
            ++word_num;
            data.text.push_back(x);
            data.pos.push_back({line_num, word_num});
        }
    }
    return true;
}

std::vector<int> search_apostolico_giancarlo(
    const std::vector<uint>& pattern,
    const std::vector<uint>& text
) {
    const int m = static_cast<int>(pattern.size());
    const int n = static_cast<int>(text.size());

    std::vector<int> matches;
    if (m == 0 || n < m) return matches;

    auto bad = build_bad_char(pattern);
    auto good = build_good_suffix(pattern);

    std::vector<int> skip(n, 0);
    int s = 0;

    while (s <= n - m) {
        int j = m - 1;

        while (j >= 0) {
            int t = s + j;

            if (skip[t] > 0) {
                int k = skip[t];
                
                if (k > j) {
                    j = -1;
                    break;
                } else if (k == j + 1) {
                    j = -1;
                    break;
                } else {
                    j -= k;
                    continue;
                }
            }

            if (pattern[j] == text[t]) {
                --j;
            } else {
                break;
            }
        }

        if (j < 0) {
            matches.push_back(s);
            if (s + m - 1 < n) {
                skip[s + m - 1] = m;
            }
            s += good[0];
        } else {
            int matched_len = m - 1 - j;
            if (matched_len > 0 && s + m - 1 < n) {
                skip[s + m - 1] = std::max(skip[s + m - 1], matched_len);
            }

            int bc_shift = 1;
            auto it = bad.find(text[s + j]);
            if (it != bad.end()) {
                bc_shift = std::max(1, j - it->second);
            } else {
                bc_shift = j + 1;
            }

            int gs_shift = good[j + 1];
            s += std::max(bc_shift, gs_shift);
        }
    }

    return matches;
}

void print_matches(
    const std::vector<int>& starts,
    const std::vector<std::pair<int, int>>& pos
) {
    for (int s : starts) {
        std::cout << pos[s].first << ", " << pos[s].second << '\n';
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    InputData data;
    if (!read_input(std::cin, data)) return 0;

    auto starts = search_apostolico_giancarlo(data.pattern, data.text);
    print_matches(starts, data.pos);
    return 0;
}