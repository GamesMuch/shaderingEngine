#include "gameObject.h"

// #include <glm/detail/type_quat.inl>

#include <string>

#include "model.h"
#include "mesh.h"
#include "SelfMath.h"

using Vec3 = glm::vec3;
using string = std::string;
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

    void gameObject::CreateGameObject(string Name, unsigned int shaderProgram, glm::vec3 Transform, glm::vec3 Scale, glm::vec3 Rotation) {
        ModelName = Name;
        modelShaderProgram = shaderProgram;
        ModelType = ModelType::Object3d;

        translate(Transform);
        // Simple rotatation math
        float Radian = SelfMath::Dot(Rotation);
        Vec3 Axis = normalize(SelfMath::Cross(Rotation));
        rotate(Axis, Radian);

        scale(Scale);
    }
    void gameObject::CreateQuad(string Name, glm::vec3 Transform, glm::vec3 Rotation, glm::vec3 Scale) {
        Mesh::generateQuad();
    }

}