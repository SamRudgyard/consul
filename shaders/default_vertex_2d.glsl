#version 330 core

layout (location = 0) in vec3 inputPosition;
layout (location = 2) in vec2 inputTexCoords;

out vec2 fragTexCoords;

uniform mat4 model;
uniform mat4 cameraMatrix;

void main()
{
    vec4 worldPosition = model * vec4(inputPosition, 1.0);
    fragTexCoords = mat2(1.0, 0.0, 0.0, -1.0) * inputTexCoords;
    gl_Position = cameraMatrix * worldPosition;
}
