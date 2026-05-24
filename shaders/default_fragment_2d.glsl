#version 330 core

out vec4 fragmentColour;

in vec2 fragTexCoords;

uniform sampler2D diffuse0;
uniform vec4 albedo;

void main()
{
    vec4 baseColour = texture(diffuse0, fragTexCoords);
    fragmentColour = vec4(baseColour.rgb * albedo.rgb, baseColour.a * albedo.a);
}
