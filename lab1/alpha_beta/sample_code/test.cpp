#include <fstream>
#include "node.h"

using namespace ChineseChess;

//博弈树搜索，depth为搜索深度
int alphaBeta(GameTreeNode node, int alpha, int beta, int depth, bool isMaximizer) {
    if (depth == 0) {
        return node.getEvaluationScore();
    }
    //TODO alpha-beta剪枝过程
    return 0;
}

int main() {
    std::ifstream file("../input/1.2.txt");
    std::vector<std::vector<char>> board;

    std::string line;
    int n = 0;
    while (std::getline(file, line)) {
        std::vector<char> row;

        for (char ch : line) {
            row.push_back(ch);
        }

        board.push_back(row);
        n = n + 1;
        if (n >= 10) break;
    }

    file.close();
    GameTreeNode root(true, board, std::numeric_limits<int>::min());

    //TODO
    // alphaBeta(root, )


    //代码测试
    ChessBoard _board = root.getBoardClass();
    std::vector<std::vector<char>> cur_board = _board.getBoard();

    for (int i = 0; i < cur_board.size(); i++) {
        for (int j = 0; j < cur_board[0].size(); j++) {
            std::cout << cur_board[i][j];
        }
        std::cout << std::endl;
    }

    std::vector<Move> red_moves = _board.getMoves(true);
    std::vector<Move> black_moves = _board.getMoves(false);

    for (int i = 0; i < red_moves.size(); i++) {
        std::cout << "init: " << red_moves[i].init_x <<  " " << red_moves[i].init_y << std::endl;
        std::cout << "next: " << red_moves[i].next_x <<  " " << red_moves[i].next_y << std::endl;
        std::cout << "score " << red_moves[i].score << std::endl;
    }
    for (int i = 0; i < black_moves.size(); i++) {
        std::cout << "init: " << black_moves[i].init_x <<  " " << black_moves[i].init_y << std::endl;
        std::cout << "next: " << black_moves[i].next_x <<  " " << black_moves[i].next_y << std::endl;
        std::cout << "score " << black_moves[i].score << std::endl;
    }

    return 0;
}