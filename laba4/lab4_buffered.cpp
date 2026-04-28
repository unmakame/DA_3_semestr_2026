#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <cstdint>

using uint = uint32_t;

struct PatternData {
    std::vector<uint> pattern;
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
    for (int i = 0; i < (int)pat.size(); i++) {
        bad[pat[i]] = i;
    }
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
        i--;
        j--;
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

bool read_pattern(std::istream& in, PatternData& data) {
    std::string line;
    if (!std::getline(in, line)) return false;
    data.pattern = parse_uint_line(line);
    return !data.pattern.empty();
}

std::vector<std::pair<int, int>> apostolico_giancarlo_buffered(
    const std::vector<uint>& pattern,
    std::istream& in,
    int buffer_size
) {
    std::vector<std::pair<int, int>> all_matches;
    
    int n_pattern = (int)pattern.size();
    if (n_pattern == 0) return all_matches;

    auto bad = build_bad_char(pattern);
    auto good = build_good_suffix(pattern);

    std::vector<uint> text_buffer;
    std::vector<std::pair<int, int>> pos_buffer;
    std::vector<uint> prev_overlap;
    std::vector<std::pair<int, int>> prev_overlap_pos;

    int line_num = 0;
    int word_num = 0;
    std::string line;
    std::istringstream current_line_stream;
    bool has_current_line = false;
    uint current_word;
    int text_line_num = 1;
    int global_index = 0;
    bool first_chunk = true;
    bool reading = true;

    while (reading) {
        text_buffer.clear();
        pos_buffer.clear();

        if (!first_chunk) {
            text_buffer = prev_overlap;
            pos_buffer = prev_overlap_pos;
        }

        // Заполняем буфер до buffer_size
        while ((int)text_buffer.size() < buffer_size) {
            if (has_current_line && current_line_stream >> current_word) {
                ++word_num;
                text_buffer.push_back(current_word);
                pos_buffer.push_back({text_line_num, word_num});
            } else if (std::getline(in, line)) {
                if (!line.empty()) {
                    ++line_num;
                    current_line_stream = std::istringstream(line);
                    word_num = 0;
                    has_current_line = true;
                    text_line_num = line_num;
                } else {
                    has_current_line = false;
                }
            } else {
                has_current_line = false;
                break;
            }
        }

        if ((int)text_buffer.size() < n_pattern) break;

        // Поиск совпадений в текущем буфере
        int text_size = (int)text_buffer.size();
        int s = 0;

        while (s <= text_size - n_pattern) {
            int j = n_pattern - 1;

            while (j >= 0 && pattern[j] == text_buffer[s + j])
                j--;

            if (j < 0) {
                // Совпадение найдено - проверяем границы строк
                if (pos_buffer[s].first == pos_buffer[s + n_pattern - 1].first) {
                    all_matches.push_back(pos_buffer[s]);
                }
                s += good[0];
            } else {
                int bc_shift;
                auto it = bad.find(text_buffer[s + j]);
                if (it != bad.end())
                    bc_shift = std::max(1, j - it->second);
                else
                    bc_shift = j + 1;

                int gs_shift = good[j + 1];
                s += std::max(bc_shift, gs_shift);
            }
        }

        prev_overlap.clear();
        prev_overlap_pos.clear();

        // Сохраняем перекрытие для следующей итерации
        int overlap_start = std::max(0, text_size - n_pattern + 1);
        for (int i = overlap_start; i < text_size; ++i) {
            prev_overlap.push_back(text_buffer[i]);
            prev_overlap_pos.push_back(pos_buffer[i]);
        }

        if ((int)text_buffer.size() < buffer_size) {
            reading = false;
        }

        first_chunk = false;
    }

    return all_matches;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    PatternData pattern_data;

    if (!read_pattern(std::cin, pattern_data))
        return 0;

    int m = (int)pattern_data.pattern.size();
    int buffer_size = m * 2 + m;  // O(m*3) память

    std::vector<std::pair<int, int>> matches =
        apostolico_giancarlo_buffered(pattern_data.pattern, std::cin, buffer_size);

    for (const auto& match : matches) {
        std::cout << match.first << ", " << match.second << '\n';
    }

    return 0;
}
