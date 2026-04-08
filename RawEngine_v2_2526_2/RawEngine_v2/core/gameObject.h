#pragma once

#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "assimpLoader.h"
#include "model.h"

enum class ModelType {Object2d,Object3d};
std::string ModelName;


namespace core {
    class GameObject2d {
    public:
        struct Obj2d{
            glm::vec3 pos;
            glm::vec3 rot;
            glm::vec3 scale;
            std::string path;
        };

        // GameObject() {}

        GameObject2d(std::string meshPath, glm::vec3 position, glm::vec3 rotation) : model(core::AssimpLoader::loadModel(meshPath)) {
            this->position = position;
            this->rotation = rotation;
            this->type = ModelType::Object3d;
        }
    };
    class GameObject3d {
        GameObject3d(std::string textureName, glm::vec3 position) : model(core::AssimpLoader::loadModel("models/quad")) {
            this->type = ModelType::Object2d;
        };
    }
}

