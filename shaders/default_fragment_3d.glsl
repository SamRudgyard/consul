#version 330 core

out vec4 fragmentColour;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;

uniform sampler2D diffuse0;
uniform sampler2D specular0;

uniform vec4 meshTint;

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform vec3 lightColour;
uniform vec3 ambientColour;
uniform int useLighting;

void main()
{
    vec3 diffuseColour = texture(diffuse0, fragTexCoords).rgb * meshTint.rgb;
    if (useLighting == 0) {
        fragmentColour = vec4(diffuseColour, meshTint.a);
        return;
    }

    // Normalise interpolated normal
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(lightPosition - fragPosition);
    vec3 viewDir = normalize(cameraPosition - fragPosition);
    vec3 reflectDir = reflect(-lightDir, normal);

    // Diffuse component
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular component
    float specStrength = texture(specular0, fragTexCoords).r; // usually grayscale
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0) * specStrength;

    vec3 ambient = ambientColour * diffuseColour;
    vec3 lighting = ambient + lightColour * (diff * diffuseColour + spec);
    
    fragmentColour = vec4(lighting, meshTint.a);
}
