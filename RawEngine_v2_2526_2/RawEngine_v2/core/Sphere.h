#ifndef SPHERE_H
#define SPHERE_H

#include "rayHit.h"

class Sphere : public hittable {



public:
    vec3 center;
    double radius;

    bool hit(const rayCast& r, double ray_tmin, double ray_tmax, hitRecord& rec) const override {
        vec3 oc = center - r.origin();
        auto a = dot(r.direction(),r.direction());
        auto h = dot(r.direction(), oc);
        auto c = dot(oc,oc) - radius*radius;

        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (root <= ray_tmin || ray_tmax <= root) {
            root = (h + sqrtd) / a;
            if (root <= ray_tmin || ray_tmax <= root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        rec.normal = (rec.p - center) / float(radius);
        vec3 outward_normal = (rec.p - center) / float(radius);
        rec.set_face_normal(r, outward_normal);

        return true;
    };
    void transform(vec3 move) {
        center += move;
    }
    void scale(int scale) {
        radius = scale;
    }
};
#endif
