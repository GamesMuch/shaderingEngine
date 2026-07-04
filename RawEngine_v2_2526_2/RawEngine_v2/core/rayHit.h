#ifndef HITTABLE
#define HITTABLE

#include "rayCast.h"

class hitRecord{
public:
    point3 p;
    vec3 normal;
    double t;
    bool front_face;

    void set_face_normal(const rayCast& r, const vec3& outward_normal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual ~hittable() = default;

    virtual bool hit(const rayCast& r, double ray_tmin, double ray_tmax, hitRecord& rec) const = 0;


};

#endif
