#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <tuple>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Board.h"
using BOARD_RESULT = std::tuple<int, int, int>;
struct BoardData {
    int index;
    BitBoard board;
};


std::queue<BoardData> globalQueue;  // Global queue to hold boards
std::string resultFile = "output.txt";

const int MAX_COMBO = 5;

BOARD_RESULT check_win(const BitBoard& board) {
    std::vector<std::pair<int, int>> directions = {{0, 1}, {1, 0}, {1, 1}, {-1, 1}};
    for (int i = 0; i < BitBoard::SIZE; ++i) {
        for (int j = 0; j < BitBoard::SIZE; ++j) {
            int color = board.get(i, j);
            if (color != 0) {
                for (const auto& [dx, dy] : directions) {
                    int combo = 1;
                    int x = i, y = j;
                    while (x + dx >= 0 && x + dx < BitBoard::SIZE && y + dy >= 0 && y + dy < BitBoard::SIZE && board.get(x + dx, y + dy) == color) {
                        combo++;
                        x += dx;
                        y += dy;
                    }
                    x = i, y = j;
                    while (x - dx >= 0 && x - dx < BitBoard::SIZE && y - dy >= 0 && y - dy < BitBoard::SIZE && board.get(x - dx, y - dy) == color) {
                        combo++;
                        x -= dx;
                        y -= dy;
                    }
                    if (combo >= MAX_COMBO) {
                        return {color, i + 1, j + 1};
                    }
                }
            }
        }
    }
    return {0, 0, 0};
}

void write_to_file(const std::string& file_path, const BOARD_RESULT& results,const int& index) {
    std::ofstream file(file_path, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Failed to open file\n";
        return;
    }
    file << "Board number:" << index << "\n" << std::get<0>(results) << "\n" << std::get<1>(results) << " " << std::get<2>(results) << "\n\n";

    file.close();
}

std::vector<std::thread> threads;
std::mutex mtx;
std::condition_variable cv;

bool finishedLoading = false;  // Indicates when loading is complete

void processData() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !globalQueue.empty() || finishedLoading; });
        
        if (globalQueue.empty()) {
            if (finishedLoading) break;  // No more data will be added, stop the thread
            continue;  // Spurious wake-up, check condition again
        }

        auto currentBoard = globalQueue.front();
        globalQueue.pop();
        std::cout << "Processing board index: " << currentBoard.index << std::endl;
        currentBoard.board.print();
        auto result = check_win(currentBoard.board);
        write_to_file(resultFile, result, currentBoard.index);
        lock.unlock();  // Unlock as soon as possible
    }
}

void acceptData(const BoardData &data) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        globalQueue.push(data);
    }
    cv.notify_one();  // Notify one waiting thread
}

void loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    int boardIndex = 0;
    while (!file.eof()) {
        std::string line;
        std::getline(file, line);
        if (line.empty()) continue;
        boardIndex = std::stoi(line);
        BitBoard newBoard;
        int rHeight = 0;
        while(std::getline(file, line)) {
            if (line.empty()) 
                break;
        
            if(line.size() == BitBoard::SIZE && rHeight < BitBoard::SIZE){
                int rWidth = 0;
                std::istringstream rowStream(line);
                char symbol;
                while (rowStream.get(symbol)) {
                    if (isdigit(symbol)) {
                        uint8_t value = symbol - '0';
                        if (value > 2) 
                            continue;
                        
                        newBoard.set(rHeight, rWidth, static_cast<int>(value));
                        rWidth++;
                    }
                    if (rWidth == BitBoard::SIZE)
                        break;
                }
                if (rWidth != BitBoard::SIZE) 
                    break;
            }
            rHeight++;
        }
        if (rHeight == BitBoard::SIZE && boardIndex >= 1 && boardIndex <= 11)
            acceptData({boardIndex, newBoard});
    }
    finishedLoading = true;  // Indicate that loading is complete
    cv.notify_all();  // Wake up all waiting threads
}

int main() {
    // Start a fixed number of worker threads
    int numThreads = 4;
    threads.reserve(numThreads);
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(processData);
    }

    loadFromFile("input.txt");

    // Wait for all threads to complete
    for (auto& t : threads) {
        t.join();
    }

    return 0;
}