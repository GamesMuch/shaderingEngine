#pragma once

#include <memory>
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

        void render();
        // GLuint getVAO();
        void translate(glm::vec3 translation);
        void rotate(glm::vec3 axis, float radians);
        void scale(glm::vec3 scale);
        glm::mat4 getModelMatrix();
    };
}