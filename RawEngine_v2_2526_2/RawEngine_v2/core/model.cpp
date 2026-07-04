#include "model.h"
#include <glm/gtc/matrix_transform.hpp>

namespace core {
    void Model::render() {
        for (int i = 0; i < meshes.size(); ++i) {
            meshes[i].render();
        }
    }
    // GLuint Model::getVAO() {
    //     return this->getVAO();
    // }

    void Model::translate(glm::vec3 translation) {
        modelMatrix = glm::translate(modelMatrix, translation);
        position += translation;
    }

    void Model::rotate(glm::vec3 axis, float radians) {
        modelMatrix = glm::rotate(modelMatrix, radians, axis);
    }

    void Model::scale(glm::vec3 scale) {
        modelMatrix = glm::scale(modelMatrix, scale);
        radius = scale.x/2;
    }

    glm::mat4 Model::getModelMatrix() const {
        return this->modelMatrix;
    }

}