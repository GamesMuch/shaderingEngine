#include "SelfMath.h"

using Vec3 = glm::vec3;

namespace core {

    float SelfMath::Dot(Vec3 a, Vec3 b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    Vec3 SelfMath::Cross(Vec3 a, Vec3 b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
            };
    }
};
