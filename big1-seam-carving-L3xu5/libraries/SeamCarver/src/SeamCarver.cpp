#include <SeamCarver.hpp>
#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <unordered_map>

SeamCarver::SeamCarver(Image image) : m_image(std::move(image)) {}

const Image &SeamCarver::GetImage() const {
    return m_image;
}

size_t SeamCarver::GetImageWidth() const {
    return m_image.m_table.size();
}

size_t SeamCarver::GetImageHeight() const {
    return m_image.m_table[0].size();
}

double SeamCarver::GetPixelEnergy(size_t columnId, size_t rowId) const {
    Image::Pixel dx =
        (columnId > 0 ? m_image.GetPixel(columnId - 1, rowId) : m_image.GetPixel(GetImageWidth() - 1, rowId)) -
        (columnId < GetImageWidth() - 1 ? m_image.GetPixel(columnId + 1, rowId) : m_image.GetPixel(0, rowId));
    Image::Pixel dy =
        (rowId > 0 ? m_image.GetPixel(columnId, rowId - 1) : m_image.GetPixel(columnId, GetImageHeight() - 1)) -
        (rowId < GetImageHeight() - 1 ? m_image.GetPixel(columnId, rowId + 1) : m_image.GetPixel(columnId, 0));
    return std::sqrt(dx.pow2delta() + dy.pow2delta());
}

template <>
struct std::hash<std::pair<int, int>> {
    size_t operator()(const std::pair<int, int> &pair) const {
        return std::hash<int>{}(pair.first) ^ std::hash<int>{}(pair.second);
    }
};

SeamCarver::Seam SeamCarver::FindSeam(bool isHorizontal) const {
    std::unordered_map<std::pair<int, int>, std::vector<std::pair<double, std::pair<int, int>>>> edges;
    std::unordered_map<std::pair<int, int>, double> d;
    size_t height = GetImageHeight();
    size_t width  = GetImageWidth();
    d[{-1, -1}]   = 0.;

    for (size_t i = 0; i < (isHorizontal ? height : width); i++) {
        edges[{-1, -1}].push_back(
            {GetPixelEnergy(isHorizontal ? 0 : i, isHorizontal ? i : 0), {isHorizontal ? 0 : i, isHorizontal ? i : 0}});
        d[{isHorizontal ? width - 1 : i, isHorizontal ? i : height - 1}] = std::numeric_limits<double>::infinity();
    }

    for (size_t i = 0; i < (isHorizontal ? width - 1 : width); i++) {
        for (size_t j = 0; j < (isHorizontal ? height : height - 1); j++) {
            double energy          = GetPixelEnergy(isHorizontal ? i + 1 : i, isHorizontal ? j : j + 1);
            std::pair<int, int> to = {isHorizontal ? i + 1 : i, isHorizontal ? j : j + 1};
            edges[{i, j}].push_back({energy, to});
            d[{i, j}] = std::numeric_limits<double>::infinity();

            if (isHorizontal ? j > 0 : i > 0) {
                edges[{i, j}].push_back({GetPixelEnergy(isHorizontal ? i + 1 : i - 1, isHorizontal ? j - 1 : j + 1),
                                         {isHorizontal ? i + 1 : i - 1, isHorizontal ? j - 1 : j + 1}});
            }
            if (isHorizontal ? j < height - 1 : i < width - 1) {
                edges[{i, j}].push_back({GetPixelEnergy(i + 1, j + 1), {i + 1, j + 1}});
            }
        }
    }

    std::set<std::pair<double, std::pair<int, int>>> s;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>> parents;
    s.insert({0, {-1, -1}});
    while (!s.empty()) {
        std::pair<int, int> v = s.begin()->second;
        s.erase(s.begin());
        for (auto [energy, to] : edges[v]) {
            if (d[to] > d[v] + energy) {
                s.erase({d[to], to});
                d[to]       = d[v] + energy;
                parents[to] = v;
                s.insert({d[to], to});
            }
        }
    }

    double bestWayEnergy = std::numeric_limits<double>::infinity();
    std::pair<int, int> endOfSeam;
    for (size_t i = 0; i < (isHorizontal ? height : width); i++) {
        std::pair<int, int> cur = {isHorizontal ? width - 1 : i, isHorizontal ? i : height - 1};
        if (bestWayEnergy > d[cur]) {
            bestWayEnergy = d[cur];
            endOfSeam     = cur;
        }
    }

    Seam seam;
    while (parents.contains(endOfSeam)) {
        seam.push_back(isHorizontal ? endOfSeam.second : endOfSeam.first);
        endOfSeam = parents[endOfSeam];
    }
    std::reverse(seam.begin(), seam.end());
    return seam;
}

SeamCarver::Seam SeamCarver::FindHorizontalSeam() const {
    return FindSeam(true);
}

SeamCarver::Seam SeamCarver::FindVerticalSeam() const {
    return FindSeam(false);
}

void SeamCarver::RemoveHorizontalSeam(const Seam &seam) {
    size_t height = GetImageHeight();
    size_t width  = GetImageWidth();

    for (size_t i = 0; i < width; i++) {
        size_t j = seam[i];
        if (j < height - 1) {
            std::swap(m_image.m_table[i][j], m_image.m_table[i][height - 1]);
        }
    }

    for (size_t i = 0; i < width; i++) {
        m_image.m_table[i].pop_back();
    }
}

void SeamCarver::RemoveVerticalSeam(const Seam &seam) {
    size_t height = GetImageHeight();
    size_t width  = GetImageWidth();

    for (size_t i = 0; i < height; i++) {
        size_t j = seam[i];
        if (j < width - 1) {
            std::swap(m_image.m_table[j][i], m_image.m_table[width - 1][i]);
        }
    }

    m_image.m_table.pop_back();
}
