#ifndef COLOR_H
#define COLOR_H

#include <glm/vec3.hpp>

using color = glm::vec3;

#include <iostream>

inline void write_color(std::ostream& out, const color& pix) {
    auto r = pix.x;
    auto g = pix.y;
    auto b = pix.z;

    int rbyte = int(255.999 * r);
    int gbyte = int(255.999 * g);
    int bbyte = int(255.999 * b);

    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif