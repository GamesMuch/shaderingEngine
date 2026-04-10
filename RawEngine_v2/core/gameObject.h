#pragma once


#include <glm/vec3.hpp>
#include "model.h"
#include <string>

using string = std::string;

namespace core {

    class gameObject {

    public:
        enum class ModelType {Object2d,Object3d};
        ModelType ModelType = ModelType::Object3d;
        std::string ModelName = "Default";

        Model model;
        unsigned int modelShaderProgram = 0;

        gameObject(Model modl) : model(modl){}

        void render();
        void translate(glm::vec3 translate);
        void rotate(glm::vec3 axis, float radians);
        void scale(glm::vec3 scale);
        glm::mat4 getModelMatrix();
        void CreateGameObject(string Name, unsigned int shaderProgram,glm::vec3 Transform, glm::vec3 Scale, glm::vec3 Rotation = glm::vec3(0,0,0));
        void CreateQuad(string Name, glm::vec3 Transform, glm::vec3 Rotation, glm::vec3 Scale);
    };
}