#version 330 core

out vec4 fragmentColour;

in vec3 fragPosition;
in vec3 fragNormal;
in vec3 fragColour;
in vec2 fragTexCoords;

uniform sampler2D diffuse0;
uniform sampler2D specular0;

void main()
{
    fragmentColour = vec4(fragColour, 1.0) * (texture(diffuse0, fragTexCoords) + texture(specular0, fragTexCoords));
}