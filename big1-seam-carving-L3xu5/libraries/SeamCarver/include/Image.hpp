#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <iostream>
#include <vector>

struct Image {
    struct Pixel {
        Pixel();
        Pixel(int red, int green, int blue);

        int m_red   = 0;
        int m_green = 0;
        int m_blue  = 0;

        const Pixel operator-(const Pixel& rv) const {
            return {m_red - rv.m_red, m_green - rv.m_green, m_blue - rv.m_blue};
        }

        double pow2delta() const { return m_red * m_red + m_blue * m_blue + m_green * m_green; }
    };

    Image(std::vector<std::vector<Pixel>> table);

    Pixel GetPixel(size_t columnId, size_t rowId) const;

    std::vector<std::vector<Pixel>> m_table;
};

#endif  // IMAGE_HPP
