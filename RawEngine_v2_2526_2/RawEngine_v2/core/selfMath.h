// #include <glm/exponential.hpp>
// #include <glm/vec3.hpp>
//
//
// using vec3 = glm::vec3;
//
// float dot(vec3 a, vec3 b(a)) {
//     a = normalize(a);
//     b = normalize(b);
//     return a.x * b.x + a.y * b.y + a.z * b.z;
// }
// vec3 cross(vec3 a, vec3 b) {
//     float x = a.y * b.z - a.z * b.y;
//     float y = a.z * b.x - a.x * b.z;
//     float z = a.x * b.y - a.y * b.x;
//     return vec3(x, y, z);
// }
// vec3 normalize(vec3 a) {
//     float length = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
//     if (length == 0){ return vec3(0);}
//     return a / length;
// }