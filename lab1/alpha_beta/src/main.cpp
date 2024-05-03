#include <iostream>
#include <format>
#include <vector>
#include <fstream>

#include "chineseChess.h"

using namespace ChineseChess;

typedef struct Node {
    Board cur;
    bool player;
    std::vector<Node> children;
} Node;

void print_board(const Board &board) {
    for (int j = BOARD_SIZE_W - 1; j >= 0; j--) {
        for (int i = 0; i < BOARD_SIZE_H; i++) {
            std::cout << to_char(board[i][j]);
        }
        std::cout << std::endl;
    }
}

int main() {
    for (int index = 1; index <= 10; index++) {
        if (index != 2) continue;
        auto input_filename = std::format("./input/{}.txt", index);
        std::cout << input_filename << std::endl;
        auto output_filename = std::format("./output/output_{}.txt", index);
        std::ifstream file(input_filename);

        auto base_board = Board();
        char ch;
        for (int j = BOARD_SIZE_W - 1; j >= 0; j--) {
            for (int i = 0; i < BOARD_SIZE_H; i++) {
                file >> ch;
                if (ch != '.') {
                    base_board[i][j] = from_char(ch);
                }
            }
        }
        file.close();

        auto boards = next_boards(base_board, false);
        for (const auto &board: boards) {
            print_board(board);
            std::cin.get();
            std::cout << std::endl << "----------------" << std::endl;
        }
    }
}