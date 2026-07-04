#include <glm/vec3.hpp>
#include <glm/ext/quaternion_geometric.hpp>
// #include <glm/detail/func_geometric.inl>

using vec3 = glm::vec3;
using point3 = vec3;


class ray {

public:
    ray() {}

    ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}

    const point3& origin() const  { return orig; }
    const vec3& direction() const { return dir; }

    point3 at(double t) const {
        return orig + (float)t * dir;
    }
    bool hit_sphere(const point3& center, double radius, const ray& r) {
        vec3 oc = center - r.origin();
        auto a = dot(r.direction(), r.direction());
        auto b = -2.0 * dot(r.direction(), oc);
        auto c = dot(oc, oc) - radius*radius;
        auto discriminant = b*b - 4*a*c;
        return (discriminant >= 0);
    }


    private:
    point3 orig;
    vec3 dir;

};