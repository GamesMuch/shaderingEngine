#version 400 core
in vec2 vuv;
out vec4 FragColor;
uniform sampler2D textureUniform;

void main()
{
    FragColor = texture(textureUniform, vuv);
}