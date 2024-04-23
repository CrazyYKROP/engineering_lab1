#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <tuple>

const int WIDTH = 19;
const int HEIGHT = 19;

std::vector<std::pair<int, std::vector<std::vector<int>>>> read_input_file(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file\n";
        return {};
    }

    std::vector<std::pair<int, std::vector<std::vector<int>>>> boards;
    while (!file.eof()) {
        std::string line;
        std::getline(file, line);
        if(line.empty()) continue;
        int index = std::stoi(line);
        std::vector<std::vector<int>> board(HEIGHT, std::vector<int>(WIDTH));
        for (int i = 0; i < HEIGHT; ++i) {
            std::getline(file, line);
            for (int j = 0; j < WIDTH; ++j) {
                board[i][j] = line[j] - '0';
            }
        }

        boards.push_back({index, board});
    }

    file.close();
    return boards;
}

std::tuple<int, int, int> check_win(const std::vector<std::vector<int>>& board) {
    std::vector<std::pair<int, int>> dir = {{0, 1}, {1, 0}, {1, 1}, {-1, 1}};
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            if (board[i][j] != 0) {
                int color = board[i][j];
                for (auto& [dx, dy] : dir) {
                    int combo = 1;
                    int x = i, y = j;
                    while (x + dx >= 0 && x + dx < HEIGHT && y + dy >= 0 && y + dy < WIDTH && board[x + dx][y + dy] == color) {
                        combo++;
                        x += dx;
                        y += dy;
                    }
                    x = i, y = j;
                    while (x - dx >= 0 && x - dx < HEIGHT && y - dy >= 0 && y - dy < WIDTH && board[x - dx][y - dy] == color) {
                        combo++;
                        x -= dx;
                        y -= dy;
                    }
                    if (combo >= 5) {
                        return {color, i + 1, j + 1};
                    }
                }
            }
        }
    }
    return {0, 0, 0};
}

void write_to_file(const std::string& file_path, const std::vector<std::pair<int, std::tuple<int, int, int>>>& results) {
    std::ofstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file\n";
        return;
    }
    for(const auto& [index, result] : results)
        file << "Board number:" << index << "\n" << std::get<0>(result) << "\n" << std::get<1>(result) << " " << std::get<2>(result) << "\n\n";

    file.close();
}

int main() {
    auto boards = read_input_file("input.txt");
    std::vector<std::pair<int, std::tuple<int, int, int>>> results;
    for (const auto& [index, board] : boards) 
        results.push_back(std::pair(index, check_win(board)));
    
    write_to_file("output.txt", results);
    return 0;
}