#include <iostream>
#include <vector>
#include <format>
#include <fstream>
#include <algorithm>

#define PLANE 0
#define BLOCK 1
#define SUPPLY 2
#define START 3
#define GOAL 4

using MapRow = std::vector<int>;
using Map = std::vector<MapRow>;
using Pos = std::pair<int, int>;
using Status = std::tuple<Pos, int, int, std::string>; // (pos, health, step, steps_str)
using NextMove = std::pair<std::string, Pos>; // (status, step_str)

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

bool pos_health_eq(const Status &lhs, const Status &rhs) {
    return std::get<0>(lhs) == std::get<0>(rhs) && std::get<1>(lhs) == std::get<1>(rhs);
}

std::optional<std::pair<int, std::string>> handle_map(const Map &map, int T) {
    auto goal = find(map, GOAL);
    auto h = [goal](const Pos &p) { return norm1(p, goal); };
    auto f = [h](const Status &s) { return std::get<2>(s) + h(std::get<0>(s)); };

    auto filter = [map](const Pos &p) {
        return p.first >= 0 && p.first < size(map).first && p.second >= 0 && p.second < size(map).second;
    };

    auto next = [filter](const Pos &p) {
        static std::vector<NextMove> next_moves = {{"R", {0,  1}},
                                                   {"L", {0,  -1}},
                                                   {"U", {-1, 0}},
                                                   {"D", {1,  0}},};

        std::vector<NextMove> result;

        for (const auto &next_move: next_moves) {
            if (auto pos = p + next_move.second; filter(pos)) {
                result.emplace_back(next_move.first, pos);
            }
        }

        return result;
    };

    auto cmp = [&](const Status &s1, const Status &s2) { return f(s1) < f(s2); };

    std::vector<Status> frontiers;
    std::vector<Status> visited;
    frontiers.emplace_back(find(map, START), T, 0, "");
    while (!frontiers.empty()) {
        std::sort(frontiers.begin(), frontiers.end(), cmp);

        auto cur = frontiers.front();
        auto [pos, health, step, steps_str] = cur;
        frontiers.erase(frontiers.begin());

        if (map[pos.first][pos.second] == GOAL)
            return {std::make_pair(step, steps_str)};

        visited.push_back(cur);

        for (const auto &next_move: next(pos)) {
            auto next_pos = next_move.second;
            if (map[next_pos.first][next_pos.second] == BLOCK)
                continue;

            int next_health = health - 1;
            if (next_health < 0) continue;

            if (map[next_pos.first][next_pos.second] == SUPPLY)
                next_health = T;

            Status next_status = {next_pos, next_health, step + 1, steps_str + next_move.first};

            if (std::find_if(visited.begin(), visited.end(),
                             [&](const Status &s) { return pos_health_eq(s, next_status); }) !=
                visited.end())
                continue;

            if (auto it = std::find_if(frontiers.begin(), frontiers.end(),
                                       [&](const Status &s) { return pos_health_eq(s, next_status); });
                    it != frontiers.end()) {
                if (std::get<2>(*it) > std::get<2>(next_status)) {
                    *it = next_status;
                }
            } else {
                frontiers.push_back(next_status);
            }
        }
    }


    return std::nullopt;
}

int main() {
    for (int index = 0; index <= 10; index++) {
        auto input_filename = std::format("./input/input_{}.txt", index);
        std::cout << input_filename << std::endl;
        auto output_filename = std::format("./output/output_{}.txt", index);
        int M, N, T;
        auto input_file = std::ifstream(input_filename);
        auto output_file = std::ofstream(output_filename);
        input_file >> M >> N >> T;

        auto map = Map(M, MapRow(N));
        for (int j = 0; j < M; j++) {
            for (int i = 0; i < N; i++) {
                input_file >> map[j][i];
            }
        }
        input_file.close();

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