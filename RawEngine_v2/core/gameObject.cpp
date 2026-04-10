#include "gameObject.h"

#include <glm/detail/type_quat.inl>

#include "model.h"
#include "mesh.h"
#include "SelfMath.h"

using Vec3 = glm::vec3;

namespace core {

    void gameObject::render() {
        model.render();
    }
    void gameObject::translate(glm::vec3 translation) {
        model.translate(translation);
    }
    void gameObject::rotate(glm::vec3 axis, float radians) {
        model.rotate(axis, radians);
    }
    void gameObject::scale(glm::vec3 translation) {
        model.scale(translation);
    }
    glm::mat4 gameObject::getModelMatrix() {
        return model.getModelMatrix();
    }

    void gameObject::CreateGameObject(std::string Name, glm::vec3 Transform, glm::vec3 Scale, glm::vec3 Rotation) {
        model.ModelName = Name;
        model.translate(Transform);
        // Simple rotatation math
        float Radian = Math::Dot(Vec3(0), Rotation);
        Vec3 Axis = Math::Cross(Vec3(0), Rotation);



        model.scale(Scale);
    }
    void gameObject::CreateQuad(std::string Name, glm::vec3 Transform, glm::vec3 Rotation, glm::vec3 Scale) {
        Mesh::generateQuad();
    }

}