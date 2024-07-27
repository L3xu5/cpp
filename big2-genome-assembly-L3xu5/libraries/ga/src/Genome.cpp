#include "ga/Genome.hpp"

#include <unordered_map>

namespace std {

template <>
struct hash<pair<size_t, size_t>> {
    size_t operator()(const pair<size_t, size_t>& p) const {
        hash<size_t> hasher;
        size_t h1 = hasher(p.first);
        size_t h2 = hasher(p.second);
        return h1 ^ (h2 << 1);
    }
};
}  // namespace std

namespace genome {

std::vector<std::size_t> findWay(const std::size_t& start,
                                 std::unordered_map<std::size_t, std::unordered_map<std::size_t, std::size_t>>& g,
                                 const std::size_t& graphSize) {
    std::unordered_map<std::size_t, std::unordered_map<std::pair<std::size_t, std::size_t>, std::size_t>> triedOnStep;
    std::vector<std::size_t> way;
    way.push_back(start);
    std::size_t cur = start;
    while (way.size() < graphSize) {
        bool nextExists = false;
        for (auto [next, count] : g[cur]) {
            if (triedOnStep.contains(way.size())) {
                if (triedOnStep[way.size()][{cur, next}] < g[cur][next]) {
                    nextExists = true;
                    g[cur][next]--;
                    cur = next;
                    way.push_back(cur);
                    break;
                }
            } else if (g[cur][next] > 0) {
                nextExists = true;
                g[cur][next]--;
                cur = next;
                way.push_back(cur);
                break;
            }
        }
        if (!nextExists) {
            way.pop_back();
            std::size_t last = way.back();
            way.pop_back();
            triedOnStep[way.size() + 1][{last, cur}]++;
            triedOnStep.erase(way.size() + 2);
            g[last][cur]++;
            cur = last;
            way.push_back(cur);
        }
    }
    return way;
}

std::string assembly(size_t k, const std::vector<std::string>& input) {
    if (k == 0 || input.empty())
        return "";
    std::unordered_map<std::size_t, std::unordered_map<std::size_t, std::size_t>> g;
    std::size_t graphSize = 1;
    std::unordered_map<std::size_t, int> inOutEdgesDiff;
    std::hash<std::string_view> hasher;
    std::unordered_map<std::size_t, std::string> names;
    for (auto gen : input) {
        for (size_t i = k; i < gen.size(); i++) {
            std::string_view cur      = std::string_view(gen).substr(i - k, k + 1);
            std::string_view fromName = cur.substr(0, k);
            std::string_view toName   = cur.substr(cur.length() - k);
            std::size_t from          = hasher(fromName);
            std::size_t to            = hasher(toName);
            if (!inOutEdgesDiff.contains(from))
                inOutEdgesDiff[from] = 0;
            if (!inOutEdgesDiff.contains(to))
                inOutEdgesDiff[to] = 0;
            if (from != to) {
                inOutEdgesDiff[to]--;
                inOutEdgesDiff[from]++;
            }
            names[from] = fromName;
            names[to]   = toName;
            graphSize++;
            g[from][to]++;
        }
    }
    std::string result;

    for (auto [node, _] : g) {
        if (inOutEdgesDiff[node] > 0) {
            std::vector<std::size_t> way = findWay(node, g, graphSize);
            result                       = names[node].substr(0, k - 1);
            for (std::size_t step : way) {
                result += names[step].substr(k - 1);
            }
            break;
        }
    }
    return result;
}
}  // namespace genome
