#include "iostream"
#include "vector"

typedef struct {
    unsigned char data[9];
} TicTacToeBoard;

std::vector<TicTacToeBoard> next_moves(const TicTacToeBoard &board, int player) {
    std::vector<TicTacToeBoard> moves;
    for (int i = 0; i < 9; i++) {
        if (board.data[i] == 0) {
            TicTacToeBoard new_board = board;
            new_board.data[i] = player;
            moves.push_back(new_board);
        }
    }
    return moves;
}

unsigned char is_game_finished(const TicTacToeBoard &board) {
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (board.data[i * 3] != 0 && board.data[i * 3] == board.data[i * 3 + 1] &&
            board.data[i * 3] == board.data[i * 3 + 2]) {
            return board.data[i * 3];
        }
    }
    // Check columns
    for (int i = 0; i < 3; i++) {
        if (board.data[i] != 0 && board.data[i] == board.data[i + 3] && board.data[i] == board.data[i + 6]) {
            return board.data[i];
        }
    }
    // Check diagonals
    if (board.data[0] != 0 && board.data[0] == board.data[4] && board.data[0] == board.data[8]) {
        return board.data[0];
    }
    if (board.data[2] != 0 && board.data[2] == board.data[4] && board.data[2] == board.data[6]) {
        return board.data[2];
    }

    // if the board is full
    bool flag = true;
    for (unsigned char i: board.data) {
        if (i == 0) {
            flag = false;
            break;
        }
    }
    if (flag) return 0;

    return 3;
}

int total_possible_games = 0;
int total_win_for_player1 = 0;
int total_win_for_player2 = 0;
int total_draw = 0;

void play_game(TicTacToeBoard board, int player) { // NOLINT(*-no-recursion)
    unsigned char result = is_game_finished(board);
    if (result != 3) {
        total_possible_games++;
        if (result == 1) {
            total_win_for_player1++;
        } else if (result == 2) {
            total_win_for_player2++;
        } else {
            total_draw++;
        }
        return;
    }

    std::vector<TicTacToeBoard> moves = next_moves(board, player);
    for (TicTacToeBoard &move: moves) {
        play_game(move, 3 - player);
    }
}

int main() {
    TicTacToeBoard initial_board = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    play_game(initial_board, 1);

    std::cout << "Total possible games: " << total_possible_games << std::endl;
    std::cout << "Total win for player 1: " << total_win_for_player1 << std::endl;
    std::cout << "Total win for player 2: " << total_win_for_player2 << std::endl;
    std::cout << "Total draw: " << total_draw << std::endl;
}
