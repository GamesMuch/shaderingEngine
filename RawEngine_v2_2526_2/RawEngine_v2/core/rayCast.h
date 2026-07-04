#include <glm/vec3.hpp>
#include <glm/ext/quaternion_geometric.hpp>
// #include <glm/detail/func_geometric.inl>

using vec3 = glm::vec3;
using point3 = vec3;


class rayCast {

public:
    rayCast() = default;

    rayCast(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}

    const point3& origin() const  { return orig; }
    const vec3& direction() const { return dir; }

    point3 at(double t) const {
        return orig + (float)t * dir;
    }

    private:
    point3 orig;
    vec3 dir;

};