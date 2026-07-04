#version 400 core
uniform vec3 lightDirection;
// TODO: add other stuff. sharpness, albedo, light color, .... etc.

out vec4 FragColor;
in vec3 fPos;
in vec3 fNor;
in vec2 uv;

void main()
{
    // just debug:
//     FragColor(lightDirection,1); // TODO: use dot product for real lighting calculations :-)
   //FragColor = vec4(fNor.x, fNor.y, fNor.z, 1);
   FragColor = vec4(1.0,1.0,1.0,1.0);
}