//
// Created by TianKai Ma on 2024/5/2.
//

#ifndef USTC_AI_24_CHINESECHESS_H
#define USTC_AI_24_CHINESECHESS_H

#include <vector>
#include <map>
#include <unordered_map>

#define BOARD_SIZE_W 10
#define BOARD_MIDDLE 5
#define BOARD_SIZE_H 9

template<typename T>
using Map = std::array<std::array<T, BOARD_SIZE_W>, BOARD_SIZE_H>;
using Pos = std::pair<int, int>;

Pos operator+(const Pos &lhs, const Pos &rhs) {
    return {lhs.first + rhs.first, lhs.second + rhs.second};
}

namespace ChineseChess {
    enum class Type {
        JU, MA, XIANG, SHI, JIANG, PAO, BING
    };

    using PType = std::pair<Type, bool>;
    using Board = Map<std::optional<PType>>;

    constexpr PType from_char(const char &c) {
        switch (c) {
            case 'r':
                return {Type::JU, false};
            case 'R':
                return {Type::JU, true};
            case 'n':
                return {Type::MA, false};
            case 'N':
                return {Type::MA, true};
            case 'b':
                return {Type::XIANG, false};
            case 'B':
                return {Type::XIANG, true};
            case 'a':
                return {Type::SHI, false};
            case 'A':
                return {Type::SHI, true};
            case 'k':
                return {Type::JIANG, false};
            case 'K':
                return {Type::JIANG, true};
            case 'c':
                return {Type::PAO, false};
            case 'C':
                return {Type::PAO, true};
            case 'p':
                return {Type::BING, false};
            case 'P':
                return {Type::BING, true};
            default:
                throw std::runtime_error("Invalid piece type");
        }
    }

    constexpr char to_char(const std::optional<PType> &type) {
        if (!type.has_value()) return '.';
        switch (type->first) {
            case Type::JU:
                return type->second ? 'R' : 'r';
            case Type::MA:
                return type->second ? 'N' : 'n';
            case Type::XIANG:
                return type->second ? 'B' : 'b';
            case Type::SHI:
                return type->second ? 'A' : 'a';
            case Type::JIANG:
                return type->second ? 'K' : 'k';
            case Type::PAO:
                return type->second ? 'C' : 'c';
            case Type::BING:
                return type->second ? 'P' : 'p';
        }
    }

    constexpr Board filter(const Board &board, const bool is_red) {
        Board result;
        for (int i = 0; i < BOARD_SIZE_H; i++) {
            for (int j = 0; j < BOARD_SIZE_W; j++) {
                if (auto piece = board[i][j]; piece.has_value() && piece->second == is_red) {
                    result[i][j] = piece;
                } else {
                    result[i][j] = std::nullopt;
                }
            }
        }
        return result;
    }

    constexpr std::vector<std::pair<Pos, Type>> filter_pieces(const Board &board, const bool is_red) {
        std::vector<std::pair<Pos, Type>> result;
        for (int i = 0; i < BOARD_SIZE_H; i++) {
            for (int j = 0; j < BOARD_SIZE_W; j++) {
                if (auto piece = board[i][j]; piece.has_value() && piece->second == is_red) {
                    result.emplace_back(std::make_pair(i, j), piece->first);
                }
            }
        }
        return result;
    }

    constexpr std::vector<std::pair<Pos, PType>> get_pieces(const Board &board) {
        std::vector<std::pair<Pos, PType>> result;
        for (int i = 0; i < BOARD_SIZE_H; i++) {
            for (int j = 0; j < BOARD_SIZE_W; j++) {
                if (auto piece = board[i][j]; piece.has_value()) {
                    result.emplace_back(std::make_pair(i, j), *piece);
                }
            }
        }
        return result;
    }

    constexpr std::vector<Type> all_types() {
        std::vector<Type> result;
        for (int i = static_cast<int>(Type::JU); i <= static_cast<int>(Type::BING); i++) {
            result.push_back(static_cast<Type>(i));
        }
        return result;
    }

    constexpr std::vector<Pos> next_moves(const Board &board, const Pos &pos, const Type &type) {
        std::vector<Pos> result;
        switch (type) {
            case Type::JU:
                for (int i = pos.first + 1; i < BOARD_SIZE_H; i++) {
                    if (auto piece = board[i][pos.second]; !piece.has_value()) {
                        result.emplace_back(i, pos.second);
                    } else {
                        // eat, if the piece is not the same color
                        if (piece->second != board[pos.first][pos.second]->second) {
                            result.emplace_back(i, pos.second);
                        }
                        break;
                    }
                }
                for (int i = pos.first - 1; i >= 0; i--) {
                    if (auto piece = board[i][pos.second]; !piece.has_value()) {
                        result.emplace_back(i, pos.second);
                    } else {
                        // eat, if the piece is not the same color
                        if (piece->second != board[pos.first][pos.second]->second) {
                            result.emplace_back(i, pos.second);
                        }
                        break;
                    }
                }
                for (int j = pos.second + 1; j < BOARD_SIZE_W; j++) {
                    if (auto piece = board[pos.first][j]; !piece.has_value()) {
                        result.emplace_back(pos.first, j);
                    } else {
                        // eat, if the piece is not the same color
                        if (piece->second != board[pos.first][pos.second]->second) {
                            result.emplace_back(pos.first, j);
                        }
                        break;
                    }
                }
                for (int j = pos.second - 1; j >= 0; j--) {
                    if (auto piece = board[pos.first][j]; !piece.has_value()) {
                        result.emplace_back(pos.first, j);
                    } else {
                        // eat, if the piece is not the same color
                        if (piece->second != board[pos.first][pos.second]->second) {
                            result.emplace_back(pos.first, j);
                        }
                        break;
                    }
                }
                break;
            case Type::MA:
                static std::vector<Pos> moves_available = {
                        {1,  2},
                        {1,  -2},
                        {-1, 2},
                        {-1, -2},
                        {2,  1},
                        {2,  -1},
                        {-2, 1},
                        {-2, -1}
                };
                static std::vector<Pos> check_if_move_possible = {
                        {0,  1},
                        {0,  -1},
                        {0,  1},
                        {0,  -1},
                        {1,  0},
                        {1,  0},
                        {-1, 0},
                        {-1, 0}
                };
                for (int i = 0; i < moves_available.size(); i++) {
                    auto move = moves_available[i];
                    auto check = check_if_move_possible[i];
                    if (pos.first + move.first >= 0 && pos.first + move.first < BOARD_SIZE_H &&
                        pos.second + move.second >= 0 && pos.second + move.second < BOARD_SIZE_W &&
                        !board[pos.first + check.first][pos.second + check.second].has_value()) {
                        if (auto piece = board[pos.first + move.first][pos.second + move.second]; !piece.has_value()) {
                            result.emplace_back(pos.first + move.first, pos.second + move.second);
                        } else {
                            // eat, if the piece is not the same color
                            if (piece->second != board[pos.first][pos.second]->second) {
                                result.emplace_back(pos.first + move.first, pos.second + move.second);
                            }
                        }
                    }
                }
                break;
            case Type::XIANG: {
                auto left_boundary = (pos.second < BOARD_MIDDLE) ? 0 : BOARD_MIDDLE;
                auto right_boundary = (pos.second < BOARD_MIDDLE) ? BOARD_MIDDLE : BOARD_SIZE_W;
                for (auto &i: {-2, 2}) {
                    for (auto &j: {-2, 2}) {
                        if (pos.first + i >= 0 && pos.first + i < BOARD_SIZE_H &&
                            pos.second + j >= left_boundary && pos.second + j < right_boundary &&
                            !board[pos.first + i / 2][pos.second + j / 2].has_value() &&
                            (!board[pos.first + i][pos.second + j].has_value() ||
                             board[pos.first + i][pos.second + j]->second != board[pos.first][pos.second]->second)) {
                            result.emplace_back(pos.first + i, pos.second + j);
                        }
                    }
                }
                break;
            }
            case Type::SHI: {
                auto left_boundary = (pos.second < BOARD_MIDDLE) ? 0 : 7;
                auto right_boundary = (pos.second < BOARD_MIDDLE) ? 3 : BOARD_SIZE_W;
                static auto top_boundary = 3;
                static auto bottom_boundary = 6;
                for (auto &i: {-1, 1}) {
                    for (auto &j: {-1, 1}) {
                        if (pos.first + i >= top_boundary && pos.first + i < bottom_boundary &&
                            pos.second + j >= left_boundary && pos.second + j < right_boundary &&
                            (!board[pos.first + i][pos.second + j].has_value() ||
                             board[pos.first + i][pos.second + j]->second != board[pos.first][pos.second]->second)) {
                            result.emplace_back(pos.first + i, pos.second + j);
                        }
                    }
                }
                break;
            }
            case Type::JIANG: {
                auto left_boundary = (pos.second < BOARD_MIDDLE) ? 0 : 7;
                auto right_boundary = (pos.second < BOARD_MIDDLE) ? 3 : BOARD_SIZE_W;
                static auto top_boundary = 3;
                static auto bottom_boundary = 6;
                for (auto &i: {-1, 0, 1}) {
                    for (auto &j: {-1, 0, 1}) {
                        if ((i == 0 && j == 0) || (i != 0 && j != 0)) continue;
                        if (pos.first + i >= top_boundary && pos.first + i < bottom_boundary &&
                            pos.second + j >= left_boundary && pos.second + j < right_boundary &&
                            (!board[pos.first + i][pos.second + j].has_value() ||
                             board[pos.first + i][pos.second + j]->second != board[pos.first][pos.second]->second)) {
                            result.emplace_back(pos.first + i, pos.second + j);
                        }
                    }
                }
                break;
            }
            case Type::PAO: {
                int pivot_i = pos.first + 1;
                // find the first non-empty piece
                while (pivot_i < BOARD_SIZE_H && !board[pivot_i][pos.second].has_value()) {
                    result.emplace_back(pivot_i, pos.second);
                    pivot_i++;
                }
                for (int j = pivot_i + 1; j < BOARD_SIZE_H; j++) {
                    if (auto piece = board[j][pos.second]; piece.has_value()) {
                        // eat, if the piece is not the same color
                        if (piece->second != board[pos.first][pos.second]->second) {
                            result.emplace_back(j, pos.second);
                        }
                        break;
                    }
                }
                pivot_i = pos.first - 1;
                // find the first non-empty piece
                while (pivot_i >= 0 && !board[pivot_i][pos.second].has_value()) {
                    result.emplace_back(pivot_i, pos.second);
                    pivot_i--;
                }
                for (int j = pivot_i - 1; j >= 0; j--) {
                    if (auto piece = board[j][pos.second]; piece.has_value()) {
                        // eat, if the piece is not the same color
                        if (piece->second != board[pos.first][pos.second]->second) {
                            result.emplace_back(j, pos.second);
                        }
                        break;
                    }
                }
                int pivot_j = pos.second + 1;
                // find the first non-empty piece
                while (pivot_j < BOARD_SIZE_W && !board[pos.first][pivot_j].has_value()) {
                    result.emplace_back(pos.first, pivot_j);
                    pivot_j++;
                }
                for (int i = pivot_j + 1; i < BOARD_SIZE_W; i++) {
                    if (auto piece = board[pos.first][i]; piece.has_value()) {
                        // eat, if the piece is not the same color
                        if (piece->second != board[pos.first][pos.second]->second) {
                            result.emplace_back(pos.first, i);
                        }
                        break;
                    }
                }
                pivot_j = pos.second - 1;
                // find the first non-empty piece
                while (pivot_j >= 0 && !board[pos.first][pivot_j].has_value()) {
                    result.emplace_back(pos.first, pivot_j);
                    pivot_j--;
                }
                for (int i = pivot_j - 1; i >= 0; i--) {
                    if (auto piece = board[pos.first][i]; piece.has_value()) {
                        // eat, if the piece is not the same color
                        if (piece->second != board[pos.first][pos.second]->second) {
                            result.emplace_back(pos.first, i);
                        }
                        break;
                    }
                }
                break;
            }
            case Type::BING: {
                // if still inside own territory, can only move forward
                // say for the red case(left side)
                if (board[pos.first][pos.second]->second && pos.second < BOARD_MIDDLE) {
                    if (pos.second + 1 >= 0 && pos.second + 1 < BOARD_SIZE_W &&
                        (!board[pos.first][pos.second + 1].has_value() ||
                         board[pos.first][pos.second + 1]->second != board[pos.first][pos.second]->second)) {
                        result.emplace_back(pos.first, pos.second + 1);
                    }
                    break;
                } else if (!board[pos.first][pos.second]->second && pos.second >= BOARD_MIDDLE) {
                    if (pos.second - 1 >= 0 && pos.second - 1 < BOARD_SIZE_W &&
                        (!board[pos.first][pos.second - 1].has_value() ||
                         board[pos.first][pos.second - 1]->second != board[pos.first][pos.second]->second)) {
                        result.emplace_back(pos.first, pos.second - 1);
                    }
                    break;
                }
                auto invalid_j_direction = (pos.second < BOARD_MIDDLE) ? -1 : 1;
                for (auto &i: {-1, 0, 1}) {
                    for (auto &j: {-1, 0, 1}) {
                        if ((j == invalid_j_direction) || (i == 0 && j == 0) || (i != 0 && j != 0)) continue;
                        if (pos.first + i >= 0 && pos.first + i < BOARD_SIZE_H &&
                            pos.second + j >= 0 && pos.second + j < BOARD_SIZE_W &&
                            (!board[pos.first + i][pos.second + j].has_value() ||
                             board[pos.first + i][pos.second + j]->second != board[pos.first][pos.second]->second)) {
                            result.emplace_back(pos.first + i, pos.second + j);
                        }
                    }
                }
                break;
            }
        }
        return result;
    }

    constexpr std::vector<Board> next_boards(const Board &board, const bool is_red) {
        std::vector<Board> result;
        auto pieces = filter_pieces(board, is_red);
        for (auto &piece: pieces) {
            auto moves = next_moves(board, piece.first, piece.second);
            for (auto &move: moves) {
                auto new_board = board;
                new_board[piece.first.first][piece.first.second] = std::nullopt;
                new_board[move.first][move.second] = board[piece.first.first][piece.first.second];
                result.push_back(new_board);
            }
        }
        return result;
    }

    constexpr std::vector<std::tuple<PType, Pos, Pos, Board>>
    next_boards_full_detail(const Board &board, const bool is_red) {
        std::vector<std::tuple<PType, Pos, Pos, Board>> result; // (piece, init_pos, next_pos, new_board)
        auto pieces = filter_pieces(board, is_red);
        for (auto &piece: pieces) {
            auto moves = next_moves(board, piece.first, piece.second);
            for (auto &move: moves) {
                auto new_board = board;
                new_board[piece.first.first][piece.first.second] = std::nullopt;
                new_board[move.first][move.second] = board[piece.first.first][piece.first.second];
                result.push_back({{piece.second, is_red}, piece.first, move, new_board});
            }
        }
        return result;
    }

    constexpr int end_check(const Board &board) {
        int red_jiang = 0, black_jiang = 0;
        for (int i = 0; i < BOARD_SIZE_H; i++) {
            for (int j = 0; j < BOARD_SIZE_W; j++) {
                if (auto piece = board[i][j]; piece.has_value() && piece->first == Type::JIANG) {
                    if (piece->second) {
                        red_jiang++;
                    } else {
                        black_jiang++;
                    }
                }
            }
        }
        if (red_jiang > 1 || black_jiang > 1) throw std::runtime_error("Invalid board state");
//        return red_jiang == 0 || black_jiang == 0;
        if (red_jiang == 0 && black_jiang == 1) return 0;
        if (red_jiang == 1 && black_jiang == 0) return 1;
        return -1;
    }

    constexpr Map<int> eval_map(const Type &type) {
        switch (type) {
            case Type::JU:
                return {{
                                {{-6, 5, -2, 4, 8, 8, 6, 6, 6, 6}},
                                {{6, 8, 8, 9, 12, 11, 13, 8, 12, 8}},
                                {{4, 6, 4, 4, 12, 11, 13, 7, 9, 7}},
                                {{12, 12, 12, 12, 14, 14, 16, 14, 16, 13}},
                                {{0, 0, 12, 14, 15, 15, 16, 16, 33, 14}},
                                {{12, 12, 12, 12, 14, 14, 16, 14, 16, 13}},
                                {{4, 6, 4, 4, 12, 11, 13, 7, 9, 7}},
                                {{6, 8, 8, 9, 12, 11, 13, 8, 12, 8}},
                                {{-6, 5, -2, 4, 8, 8, 6, 6, 6, 6}},
                        }};
            case Type::MA:
                return {{
                                {{0, -3, 5, 4, 2, 2, 5, 4, 2, 2}},
                                {{-3, 2, 4, 6, 10, 12, 20, 10, 8, 2}},
                                {{2, 4, 6, 10, 13, 11, 12, 11, 15, 2}},
                                {{0, 5, 7, 7, 14, 15, 19, 15, 9, 8}},
                                {{2, -10, 4, 10, 15, 16, 12, 11, 6, 2}},
                                {{0, 5, 7, 7, 14, 15, 19, 15, 9, 8}},
                                {{2, 4, 6, 10, 13, 11, 12, 11, 15, 2}},
                                {{-3, 2, 4, 6, 10, 12, 20, 10, 8, 2}},
                                {{0, -3, 5, 4, 2, 2, 5, 4, 2, 2}},
                        }};
            case Type::XIANG:
                return {{
                                {{0, 0, -2, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 3, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, -2, 0, 0, 0, 0, 0, 0, 0}},
                        }};
            case Type::SHI:
                return {{
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 3, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                        }};
            case Type::JIANG:
                return {{
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{1, -8, -9, 0, 0, 0, 0, 0, 0, 0}},
                                {{5, -8, -9, 0, 0, 0, 0, 0, 0, 0}},
                                {{1, -8, -9, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                        }};
            case Type::PAO:
                return {{
                                {{0, 0, 1, 0, -1, 0, 0, 1, 2, 4}},
                                {{0, 1, 0, 0, 0, 0, 3, 1, 2, 4}},
                                {{1, 2, 4, 0, 3, 0, 3, 0, 0, 0}},
                                {{3, 2, 3, 0, 0, 0, 2, -5, -4, -5}},
                                {{3, 2, 5, 0, 4, 4, 4, -4, -7, -6}},
                                {{3, 2, 3, 0, 0, 0, 2, -5, -4, -5}},
                                {{1, 2, 4, 0, 3, 0, 3, 0, 0, 0}},
                                {{0, 1, 0, 0, 0, 0, 3, 1, 2, 4}},
                                {{0, 0, 1, 0, -1, 0, 0, 1, 2, 4}},
                        }};
            case Type::BING:
                return {{
                                {{0, 0, 0, -2, 3, 10, 20, 20, 20, 0}},
                                {{0, 0, 0, 0, 0, 18, 27, 30, 30, 0}},
                                {{0, 0, 0, -2, 4, 22, 30, 45, 50, 0}},
                                {{0, 0, 0, 0, 0, 35, 40, 55, 65, 2}},
                                {{0, 0, 0, 6, 7, 40, 42, 55, 70, 4}},
                                {{0, 0, 0, 0, 0, 35, 40, 55, 65, 2}},
                                {{0, 0, 0, -2, 4, 22, 30, 45, 50, 0}},
                                {{0, 0, 0, 0, 0, 18, 27, 30, 30, 0}},
                                {{0, 0, 0, -2, 3, 10, 20, 20, 20, 0}},
                        }};
        }
    }

    constexpr int piece_value(const Type &type) {
        switch (type) {
            case Type::JU:
                return 500;
            case Type::MA:
                return 300;
            case Type::XIANG:
                return 30;
            case Type::SHI:
                return 10;
            case Type::JIANG:
                return 10000;
            case Type::PAO:
                return 300;
            case Type::BING:
                return 90;
        }
    }

    constexpr int next_move_value(const Type &type) {
        switch (type) {
            case Type::JU:
                return 500;
            case Type::MA:
                return 100;
            case Type::JIANG:
                return 10000;
            case Type::PAO:
                return 100;
            case Type::BING:
                return 20;
            default:
                return 0;

        }
    }

    constexpr int total_piece_value(const Board &board, const bool is_red) {
        int result = 0;
        auto pieces = filter_pieces(board, is_red);
        for (auto &piece: pieces) {
            result += piece_value(piece.second);
        }
        return result;
    }

    constexpr int next_move_values(const Board &board, const bool is_red) {
        int result = 0;
        auto pieces = filter_pieces(board, is_red);
        for (auto &piece: pieces) {
            auto moves = next_moves(board, piece.first, piece.second);
            for (auto &move: moves) {
                // determine whether a piece is eaten:
                if (auto piece_eaten = board[move.first][move.second]; piece_eaten.has_value()) {
                    auto tmp = next_move_value(piece_eaten->first);
                    // result = std::max(result, tmp);
                    result += tmp;
                }
            }
        }
        return result;
    }

    constexpr int total_piece_location_value(const Board &board, const bool is_red) {
        int result = 0;
        auto pieces = filter_pieces(board, is_red);
        for (auto &piece: pieces) {
            auto map = eval_map(piece.second);
            result += map[piece.first.first][piece.first.second];
        }
        return result;
    }

    constexpr int score(const Board &board) {
        switch (end_check(board)) {
            case 0:
                return std::numeric_limits<int>::min();
            case 1:
                return std::numeric_limits<int>::max();
            default:
                break;
        }

        return (total_piece_value(board, true) - total_piece_value(board, false)) +
               (total_piece_location_value(board, true) - total_piece_location_value(board, false)) +
               (next_move_values(board, true) - next_move_values(board, false)) +
               0;
    }

    constexpr std::optional<std::tuple<PType, Pos, Pos>>
    which_move(const Board &board, const bool is_red, const int given_score) {
        // evaluate all moves possible like in next_boards, and evaluate all board, return the move with the given score
        auto pieces = filter_pieces(board, is_red);
        for (auto &piece: pieces) {
            auto moves = next_moves(board, piece.first, piece.second);
            for (auto &move: moves) {
                auto new_board = board;
                new_board[piece.first.first][piece.first.second] = std::nullopt;
                new_board[move.first][move.second] = board[piece.first.first][piece.first.second];
                std::cout << "score: " << score(new_board) << std::endl;
                if (given_score == score(new_board)) {
                    return std::make_tuple(*board[piece.first.first][piece.first.second], piece.first, move);
                }
            }
        }
        return std::nullopt;
    }

} // ChineseChess

#endif //USTC_AI_24_CHINESECHESS_H
