#version 330 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec3 inputNormal;
layout (location = 2) in vec3 inputColour;
layout (location = 3) in vec2 inputTexCoords;

out vec3 fragPosition;
out vec3 fragNormal;
out vec3 fragColour;
out vec2 fragTexCoords;

uniform mat4 model;
uniform mat4 translation;
uniform mat4 scale;
uniform mat4 rotation;
uniform mat4 camMatrix;

void main()
{
    fragPosition = vec3(model * translation * rotation * scale * vec4(inputPosition, 1.0));
    fragNormal = inputNormal;
    fragColour = inputColour;
    fragTexCoords = mat2(1.0, 0.0, 0.0, -1.0)*inputTexCoords;

    gl_Position = camMatrix * vec4(fragPosition, 1.0);
}