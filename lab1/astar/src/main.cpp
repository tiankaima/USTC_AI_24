#include <iostream>
#include <vector>
#include <format>
#include <fstream>

#define PLANE 0
#define BLOCK 1
#define SUPPLY 2
#define START 3
#define GOAL 4

using MapRow = std::vector<int>;
using Map = std::vector<MapRow>;
using Pos = std::pair<int, int>;
using Status = std::tuple<Pos, int, int, std::string>; // (pos, step, health, steps_str)
using NextMove = std::pair<std::string, Pos>; // (status, step_str)
using NextMoves = std::vector<NextMove>;

using HandleResult = std::optional<std::pair<int, std::string>>;

std::pair<int, int> size(const Map &map) {
    return {map.size(), map[0].size()};
}

Pos find(const Map &map, int type) {
    for (int i = 0; i < size(map).first; i++)
        for (int j = 0; j < size(map).second; j++)
            if (map[i][j] == type)
                return {i, j};
    throw std::runtime_error("Not found");
}

int norm1(const Pos &p1, const Pos &p2) { // heuristic function
    return std::abs(p1.first - p2.first) + std::abs(p1.second - p2.second);
}

Pos operator+(const Pos &lhs, const Pos &rhs) {
    return {lhs.first + rhs.first, lhs.second + rhs.second};
}

HandleResult handle_map(const Map &map, int T) {
    auto goal = find(map, GOAL);
    auto h = [goal](const Pos &p) { return norm1(p, goal); };
    auto f = [h](const Status &s) { return std::get<1>(s) + h(std::get<0>(s)); };

    auto filter = [map](const Pos &p) {
        return p.first >= 0 && p.first < size(map).first && p.second >= 0 && p.second < size(map).second;
    };

    auto next = [filter](const Pos &p) {
        static NextMoves next_moves = {{"R", {0,  1}},
                                       {"L", {0,  -1}},
                                       {"U", {-1, 0}},
                                       {"D", {1,  0}},};

        NextMoves result;

        for (const auto &next_move: next_moves) {
            if (auto pos = p + next_move.second; filter(pos)) {
                result.push_back({next_move.first, pos});
            }
        }

        return result;
    };

    auto cmp = [&](const Status &s1, const Status &s2) { return f(s1) > f(s2); };

    std::priority_queue<Status, std::vector<Status>, decltype(cmp)> nodes(cmp);
    std::vector<Pos> visited;
    nodes.push({find(map, START), 0, T, ""});
    while (!nodes.empty()) {
        auto [pos, step, health, steps_str] = nodes.top();
        nodes.pop();
        if (map[pos.first][pos.second] == GOAL)
            return {std::make_pair(step, steps_str)};
        if (std::find(visited.begin(), visited.end(), pos) != visited.end())
            continue;
        visited.push_back(pos);

        for (const auto &next_move: next(pos)) {
            auto next_pos = next_move.second;
            if (map[next_pos.first][next_pos.second] == BLOCK)
                continue;
            int next_health = health - 1;
            if (map[next_pos.first][next_pos.second] == SUPPLY)
                next_health = T;

            if (next_health >= 0)
                nodes.push({next_pos, step + 1, next_health, steps_str + next_move.first});
        }
    }


    return std::nullopt;
}

int main() {
    for (int i = 0; i <= 10; i++) {
        auto input_filename = std::format("./input/input_{}.txt", i);
        std::cout << input_filename << std::endl;
        auto output_filename = std::format("./output/output_{}.txt", i);
        int M, N, T;
        auto input_file = std::ifstream(input_filename);
        auto output_file = std::ofstream(output_filename);
        input_file >> M >> N >> T;

        auto map = Map(M, MapRow(N));
        for (int j = 0; j < M; j++) {
            for (int k = 0; k < N; k++) {
                input_file >> map[j][k];
            }
        }

        auto result = handle_map(map, T);
        if (result.has_value()) {
            auto [cost, path] = result.value();
            std::cout << cost << std::endl;
            std::cout << path << std::endl;
            output_file << cost << std::endl;
            output_file << path << std::endl;
        } else {
            std::cout << "-1" << std::endl;
            output_file << "-1" << std::endl;
        }
    }
}