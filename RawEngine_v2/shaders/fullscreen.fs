#version 400 core

in vec2 vuv;
uniform sampler2D textureUniform;
out vec4 frag_colour;

uniform bool shaderBoolList[5];

uniform float pixelSize = 0.02;

uniform vec3 colorShader;

uniform float edgeIntensity;
uniform vec3 edgeColor;

int inputs = 0;
vec2 currentUv;
void main()
{
    currentUv = vuv;
    vec3 finalColor = vec3(0.0);

    if (shaderBoolList[2])
        {
            currentUv = floor(vuv / pixelSize) * pixelSize;
        }

    if (shaderBoolList[0] == true){
        vec2 texel = 1.0/ textureSize(textureUniform, 0);

        vec3 center = texture(textureUniform, currentUv).rgb;

        vec3 left  = texture(textureUniform, currentUv + vec2(-texel.x, 0.0)).rgb;
        vec3 right = texture(textureUniform, currentUv + vec2( texel.x, 0.0)).rgb;
        vec3 up    = texture(textureUniform, currentUv + vec2(0.0,  texel.y)).rgb;
        vec3 down  = texture(textureUniform, currentUv + vec2(0.0, -texel.y)).rgb;

        float difference = length(center - left) + length(center - right) + length(center - down) + length(center - up);

        if (difference > edgeIntensity){
            finalColor += edgeColor;
        }
        else{
            finalColor += center;
        }
        inputs++;
    }
        if (shaderBoolList[1]){
            vec4 value = texture(textureUniform, currentUv);

            vec3 color = value.xyz;

            inputs++;
            finalColor += color * colorShader;
        }
    if(inputs == 0) {
        finalColor = texture(textureUniform, currentUv).rgb;
        inputs = 1; // safe division
    }
    finalColor = finalColor / inputs;

        if (shaderBoolList[3]){
             finalColor = vec3(1.0) - finalColor;
        }
        if (shaderBoolList[4]){
            finalColor = vec3((finalColor.r + finalColor.g + finalColor.b) / 3);
        }

    frag_colour = vec4(finalColor, 1.0);
}