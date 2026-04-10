#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glm/ext/matrix_float4x4.hpp>
#include "mesh.h"

namespace core {


    class Model {
    private:
        std::vector<Mesh> meshes;
        glm::mat4 modelMatrix;
    public:
        Model(std::vector<Mesh> meshes) : meshes(meshes), modelMatrix(1) {}
        ~Model() { printf("Model was destroyed %s!\n", ModelName.c_str()); }
        enum class ModelType {Object2d,Object3d};
        std::string ModelName;
        ModelType type;


        void render();
        // GLuint getVAO();
        void translate(glm::vec3 translation);
        void rotate(glm::vec3 axis, float radians);
        void scale(glm::vec3 scale);
        glm::mat4 getModelMatrix() const;
    };
}