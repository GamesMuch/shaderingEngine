#version 400 core
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;

out vec2 vuv;

void main()
{
    vuv = uv;
    gl_Position = vec4(vertex.xy, 0.0, 1.0); // z=0 in clip space
}