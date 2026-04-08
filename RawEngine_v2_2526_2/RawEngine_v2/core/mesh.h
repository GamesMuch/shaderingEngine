#pragma once

#include <vector>
#include <glad/glad.h>
#include "vertex.h"

namespace core {
    class Mesh {
    private:
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
        ~Mesh(){ printf("Deleted a mesh.\n"); }
        void render();
        GLuint getVAO();
        static Mesh generateQuad();
    private:
        void setupBuffers();
    };
}