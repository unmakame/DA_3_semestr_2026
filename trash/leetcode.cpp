#include <vector>
#include <string>
#include <map>

using namespace std;

class Solution {
private:
    // Структура узла дерева
    struct Node {
        // Карта ребер: ключ - первый символ ребра. 
        // Значение - пара: узел, куда ведет ребро, и пара индексов [start, end) в строке text
        map<char, pair<Node*, pair<int, int>>> edges;
        
        // Количество оригинальных слов (префиксов), проходящих через этот узел
        int count = 0;
    };

    // Класс сжатого суффиксного дерева
    class CompressedSuffixTree {
    private:
        Node* root;   // Указатель на корневой узел дерева
        string text;  // Ссылка на общую склеенную строку всех слов

        // Рекурсивный метод для очистки памяти дерева (деструктор)
        void deleteTree(Node* node) {
            if (!node) return; // Если узел пустой, выходим
            for (auto& p : node->edges) {
                deleteTree(p.second.first); // Рекурсивно удаляем всех детей
            }
            delete node; // Удаляем сам узел
        }

    public:
        // Конструктор: инициализирует корень и сохраняет рабочую строку
        CompressedSuffixTree(const string& t) : root(new Node()), text(t) {}

        // Деструктор: автоматически запускает очистку памяти при уничтожении дерева
        ~CompressedSuffixTree() {
            deleteTree(root);
        }

        // Метод вставки суффикса в сжатое дерево
        void insertSuffix(int suffix_start, bool is_full_word) {
            Node* current = root;     // Начинаем обход всегда от корня
            int i = suffix_start;     // Текущий индекс вставляемого суффикса
            int n = text.length();    // Общая длина текста

            while (i < n) {           // Пока не вставили весь суффикс до конца строки
                char ch = text[i];    // Смотрим на текущий символ суффикса
                
                // Если из текущего узла нет ребра, начинающегося с символа ch
                if (current->edges.find(ch) == current->edges.end()) {
                    Node* leaf = new Node(); // Создаем новый узел-лист
                    if (is_full_word) leaf->count = 1; // Если это начало слова, ставим счетчик 1
                    current->edges[ch] = {leaf, {i, n}}; // Создаем ребро до конца текста [i, n)
                    if (is_full_word) current->count++;  // Увеличиваем счетчик у родительского узла
                    return; // Вставка этого суффикса завершена
                }

                // Если ребро с таким символом уже существует, берем его данные
                auto& edge = current->edges[ch];
                Node* next_node = edge.first;         // Узел, к которому ведет ребро
                int edge_start = edge.second.first;   // Индекс начала подстроки ребра
                int edge_end = edge.second.second;    // Индекс конца подстроки ребра

                int len = edge_end - edge_start;      // Полная длина подстроки на этом ребре
                int match_len = 0;                    // Сколько символов совпало с суффиксом

                // Сравниваем символы на ребре с текущими символами суффикса посимвольно
                while (match_len < len && i + match_len < n && text[edge_start + match_len] == text[i + match_len]) {
                    match_len++; // Увеличиваем длину совпадения
                }

                // Сценарий 1: Суффикс полностью поглотил подстроку ребра
                if (match_len == len) {
                    if (is_full_word) current->count++; // Обновляем счетчик текущего узла
                    current = next_node; // Переходим в следующий узел
                    i += len;            // Сдвигаем индекс суффикса на длину ребра
                } 
                // Сценарий 2: Частичное совпадение (требуется расщепление ребра)
                else {
                    Node* split_node = new Node(); // Создаем промежуточный узел разветвления
                    split_node->count = next_node->count; // Он наследует счетчик нижнего узла

                    // Подключаем старый узел next_node к новому split_node (с остатком старого ребра)
                    split_node->edges[text[edge_start + match_len]] = {next_node, {edge_start + match_len, edge_end}};

                    // Создаем новый лист для остатка нашего вставляемого суффикса
                    Node* leaf = new Node();
                    if (is_full_word) leaf->count = 1; // Если это начало слова, лист получает 1
                    split_node->edges[text[i + match_len]] = {leaf, {i + match_len, n}}; // Вешаем лист на split_node

                    // Изменяем старое ребро текущего узла: теперь оно ведет к split_node и укорочено
                    edge.first = split_node;
                    edge.second.second = edge_start + match_len;

                    // Если вставляли целое слово, обновляем счетчики на развилке
                    if (is_full_word) {
                        current->count++;
                        split_node->count++;
                    }
                    return; // Вставка через расщепление успешно завершена
                }
            }
            // Обработка крайнего случая, если суффикс закончился точно в узле
            if (is_full_word) current->count++;
        }

        // Метод поиска префикса и подсчета совпадений
        int countPrefix(const string& pref) {
            Node* current = root; // Начинаем поиск от корня дерева
            int i = 0;            // Индекс текущего символа искомого префикса
            int m = pref.length(); // Полная длина искомого префикса

            while (i < m) {          // Идем, пока не проверим весь префикс
                char ch = pref[i];   // Текущий искомый символ
                // Если ребра с таким символом нет — значит, префикс отсутствует в дереве
                if (current->edges.find(ch) == current->edges.end()) {
                    return 0;
                }

                // Получаем параметры подходящего ребра
                auto& edge = current->edges[ch];
                Node* next_node = edge.first;
                int edge_start = edge.second.first;
                int edge_end = edge.second.second;

                int len = edge_end - edge_start; // Длина текста на ребре
                int match_len = 0;               // Совпавшая часть на данном шаге

                // Сравниваем символы ребра с символами нашего префикса pref
                while (match_len < len && i + match_len < m && text[edge_start + match_len] == pref[i + match_len]) {
                    match_len++;
                }

                // Если мы полностью сопоставили весь префикс pref
                if (i + match_len == m) {
                    return next_node->count; // Возвращаем количество слов, прошедших через этот путь
                }

                // Если ребро закончилось, а префикс еще нет — спускаемся глубже по дереву
                if (match_len == len) {
                    current = next_node; // Переходим к следующему узлу
                    i += len;            // Продвигаем индекс поиска
                } else {
                    return 0; // Символы разошлись внутри ребра — совпадений нет
                }
            }
            return current->count; // Возврат на случай пустой строки или точного совпадения в корне
        }
    };

public:
    // Главная функция решения задачи LeetCode
    int prefixCount(vector<string>& words, const string& pref) {
        // Если массив слов пуст или префикс пустой, возвращаем 0
        if (words.empty() || pref.empty()) return 0;

        string big_text = ""; // Строка для объединения всех слов
        vector<pair<int, bool>> suffixes; // Список пар: {индекс_начала_в_big_text, флаг_начала_слова}

        // Шаг 1: Формируем единый текст и собираем координаты суффиксов
        for (const string& word : words) {
            int word_start = big_text.length(); // Запоминаем, где начинается текущее слово
            big_text += word + "$"; // Добавляем слово и разделитель в общую строку
            
            // Пробегаем по всем суффиксам добавленного слова
            for (int i = 0; i < (int)word.length(); ++i) {
                // Добавляем суффикс: (word_start + i) - его позиция. 
                // (i == 0) - true только для полноценного слова, false для его внутренних суффиксов
                suffixes.push_back({word_start + i, (i == 0)});
            }
        }

        // Шаг 2: Создаем сжатое суффиксное дерево на базе склеенной строки
        CompressedSuffixTree tree(big_text);
        
        // Шаг 3: Последовательно вставляем все собранные суффиксы в дерево
        for (const auto& suff : suffixes) {
            tree.insertSuffix(suff.first, suff.second);
        }

        // Шаг 4: Находим префикс и возвращаем результат вычислений
        return tree.countPrefix(pref);
    }
};