#version 400
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aUv;
uniform mat4 mvpMatrix;
// TODO: UNITY_MATRIX_M???
out vec3 fPos;
out vec3 fNor;
out vec2 vuv;

void main() {
  fPos = aPos; // bad!
  fNor = aNor; // bad!
  vuv = aUv;
  gl_Position = mvpMatrix * vec4(aPos, 1.0);
}