#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <tuple>
#include <queue>
#include "Board.h"

struct BoardData {
    int index;
    BitBoard board;
    std::string readResult;
};

std::queue<BoardData> globalQueue;  // Global queue to hold boards

void loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    int boardIndex = 0;
    while (!file.eof()) {
        std::string line;
        std::getline(file, line);
        if (line.empty()) continue;

        BitBoard newBoard;
        int rHeight = 0;
        bool success = true;
        std::cout << "Reading board index: " << boardIndex << std::endl;
        for (int i = 0; i < BitBoard::SIZE; i++) {
            if (!std::getline(file, line) || line.empty()) {
                success = false;
                break;
            }
            int rWidth = 0;
            std::istringstream rowStream(line);
            char symbol;
            while (rowStream.get(symbol)) {
                if (isdigit(symbol)) {
                    uint8_t value = symbol - '0';
                    if (value > 2) continue;
                    newBoard.set(rHeight, rWidth, static_cast<int>(value));
                    rWidth++;
                }
                if (rWidth == BitBoard::SIZE) break;
            }
            if (rWidth != BitBoard::SIZE) {
                success = false;
                break;
            }
            rHeight++;
        }
        if (rHeight != BitBoard::SIZE) success = false;

        globalQueue.push({ boardIndex, newBoard, success ? "Success" : "Failure" });
        boardIndex++;
    }
}

int main() {
    loadFromFile("path_to_file.txt"); // Specify the file path
    while (!globalQueue.empty()) {
        auto& bd = globalQueue.front();
        std::cout << "Board Index: " << bd.index << " - Read Result: " << bd.readResult << std::endl;
        bd.board.print();
        globalQueue.pop();
    }

    return 0;
}