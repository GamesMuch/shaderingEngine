#pragma once

#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "assimpLoader.h"
#include "model.h"



namespace core {

    class gameObject {

    public:
        enum class ModelType {Object2d,Object3d};
        ModelType ModelType;
        std::string ModelName;
        Model Model;

        void render();
        void translate(glm::vec3 translate);
        void rotate(glm::vec3 axis, float radians);
        void scale(glm::vec3 scale);
        glm::mat4 getModelMatrix();
        void CreateGameObject(std::string Name,glm::vec3 Transform, glm::vec3 Rotation, glm::vec3 Scale);
        void CreateQuad(std::string Name, glm::vec3 Transform, glm::vec3 Rotation, glm::vec3 Scale);
    };
}