#include <glm/vec3.hpp>
#pragma once

using Vec3 = glm::vec3;

namespace core {
    class Math {
        public:
        static float Dot(Vec3 a, Vec3 b = Vec3(0,0,1));
        static Vec3 Cross(Vec3 a, Vec3 b = Vec3(0,0,1));
    };
}