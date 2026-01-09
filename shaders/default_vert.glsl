#version 330 core

layout (location = 0) in vec3 inputPosition;
layout (location = 1) in vec3 inputNormal;
layout (location = 2) in vec2 inputTexCoords;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoords;

uniform mat4 model;
uniform mat4 cameraMatrix;

void main()
{
    fragPosition = vec3(model * vec4(inputPosition, 1.0));
    fragNormal = inputNormal;
    fragTexCoords = mat2(1.0, 0.0, 0.0, -1.0)*inputTexCoords;

    gl_Position = cameraMatrix * vec4(fragPosition, 1.0);
}
