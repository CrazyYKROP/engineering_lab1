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

    // Constructor that initializes all bits to 0 using memset
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