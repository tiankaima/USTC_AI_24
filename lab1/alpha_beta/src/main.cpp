#include <iostream>
#include <format>
#include <fstream>

#include "chineseChess.h"

using namespace ChineseChess;

void print_board(const Board &board) {
    for (int j = BOARD_SIZE_W - 1; j >= 0; j--) {
        for (int i = 0; i < BOARD_SIZE_H; i++) {
            std::cout << to_char(board[i][j]);
        }
        std::cout << std::endl;
    }
}

void print_pos(const Pos &pos) {
    std::cout << std::format("({},{})", pos.first, pos.second);
}

std::ostream &operator<<(std::ostream &os, const Pos &pos) {
    os << std::format("({},{})", pos.first, pos.second);
    return os;
}

#define MAX_DEPTH 4

int alpha_beta(const Board &cur, int depth, bool is_red, int alpha, int beta) { // NOLINT(*-no-recursion)
    if (depth == MAX_DEPTH) {
        return score(cur);
    }

    int value = is_red ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
    for (const auto &board: next_boards(cur, is_red)) {
        int tmp = alpha_beta(board, depth + 1, !is_red, alpha, beta);
        if (is_red) {
            if (tmp > value) {
                value = tmp;
            }
            if (beta >= value) {
                return value;
            }
            alpha = std::max(alpha, value);
        } else {
            if (tmp < value) {
                value = tmp;
            }
            if (alpha <= value) {
                return value;
            }
            beta = std::min(beta, value);
        }
    }

    return value;
}

std::tuple<PType, Pos, Pos> alpha_beta_start(const Board &cur) {
    int value = std::numeric_limits<int>::min();
    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();

    PType type_;
    Pos prev_pos_, next_pos_;
    for (const auto &next: next_boards_full_detail(cur, true)) {
        const auto &[type, prev_pos, next_pos, board] = next;
        int tmp = alpha_beta(board, 1, false, alpha, beta);

        if (tmp > value) {
            type_ = type;
            prev_pos_ = prev_pos;
            next_pos_ = next_pos;
            value = tmp;
        }
//        if (beta >= value) {
//            return {type_, prev_pos_, next_pos_};
//        }
        alpha = std::max(alpha, value);
    }

    return {type_, prev_pos_, next_pos_};
}


int main() {
    for (int index = 1; index <= 10; index++) {
//        if (index != 1) continue; // TODO: remove this.

        auto input_filename = std::format("./input/{}.txt", index);
        std::cout << input_filename << std::endl;
        auto output_filename = std::format("./output/output_{}.txt", index);
        std::ifstream file(input_filename);
        std::ofstream output(output_filename);

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

        auto [type, prev, next] = alpha_beta_start(base_board);

        output << to_char(type) << " " << prev << " " << next << std::endl;
        std::cout << to_char(type) << " " << prev << " " << next << std::endl;

        output.close();
    }
}