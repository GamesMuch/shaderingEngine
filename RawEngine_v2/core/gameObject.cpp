#include "gameObject.h"
#include "model.h"
#include "mesh.h"

namespace core {
    void gameObject::translate(glm::vec3 translation) {
        Model.translate(translation);
    }
    void gameObject::rotate(glm::vec3 axis, float radians) {
        Model.rotate(axis, radians);
    }
    void gameObject::scale(glm::vec3 translation) {
        Model.scale(translation);
    }
    glm::mat4 gameObject::getModelMatrix() {
        return Model.getModelMatrix();
    }


    void gameObject::CreateGameObject(std::string Name, glm::vec3 Transform, glm::vec3 Rotation, float RotationRadians, glm::vec3 Scale) {

    }
    void gameObject::CreateQuad(std::string Name, glm::vec3 Transform, glm::vec3 Rotation, glm::vec3 Scale) {
        Mesh::generateQuad();
    }

}