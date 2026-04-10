#include "glm/detail/type_quat.hpp"
#include "SelfMath.h"

using Vec3 = glm::vec3;

namespace core {
    class SelfMath {
    public:
        static float Dot(Vec3 a, Vec3 b) {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        static Vec3 Cross(Vec3 a, Vec3 b) {
            a = normalize(a);
            b = normalize(b);

            return {
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.y * b.x - a.x * b.y
                };
        }
    };
}