#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <cstring> // Include for memset
#include <string>
#include <sstream>
#include <cctype>
#include <cstdint>  // For uint8_t

struct BitBoard {
    static const int SIZE = 19;
    unsigned int data[23]; // Enough to store 19x19 board with 2 bits per cell

    // Constructor that initializes all bits to 0
    BitBoard() {
        memset(data, 0, sizeof(data));
    }

    // Function to set the value at a particular row and column
    void set(int row, int col, int value) {
        int index = (row * SIZE + col) * 2;
        int arr_index = index / 32;
        int bit_index = index % 32;
        // Clear the current bits
        data[arr_index] &= ~(3 << bit_index);
        // Set new value
        data[arr_index] |= (value & 3) << bit_index;
    }

    // Function to get the value at a particular row and column
    int get(int row, int col) const {
        int index = (row * SIZE + col) * 2;
        int arr_index = index / 32;
        int bit_index = index % 32;
        return (data[arr_index] >> bit_index) & 3;
    }

    // Function to load the board from a file
    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        while (!file.eof()) {
            std::string line;
            std::getline(file, line);
            if (line.empty()) continue;
            int rHeight = 0;
            for (int i = 0; i < SIZE; i++) {
                std::string row;
                std::getline(file, row);
                if (row.empty()) {
                    std::cerr << "Empty row parsing" << std::endl;
                    return;
                }
                int rWidth = 0;
                std::istringstream rowStream(row);
                char symbol;
                while (rowStream.get(symbol)) {
                    if (isdigit(symbol)) {
                        uint8_t value = symbol - '0';
                        if (value > 2) {
                            std::cerr << "Unexpected digit" << std::endl;
                            continue;
                        }
                        set(rHeight, rWidth, static_cast<int>(value));
                        rWidth++;
                    }
                    if (rWidth == SIZE) break;
                }
                if (rWidth != SIZE) {
                    std::cerr << "Unexpected row length" << std::endl;
                    return;
                }
                rHeight++;
            }
            if (rHeight != SIZE) {
                std::cerr << "Unexpected board height" << std::endl;
                return;
            }
        }
    }
    // Function to print the board to console
    void print() const {
        for (int r = 0; r < SIZE; ++r) {
            for (int c = 0; c < SIZE; ++c) {
                std::cout << get(r, c) << " ";
            }
            std::cout << std::endl;
        }
    }
};
