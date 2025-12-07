#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

const vec3 lightPos = vec3(0.0, 4.0, 0.0);   // world space
const vec3 viewPos = vec3(0.0, 0.0, 5.0);    // TODO: move to uniform

void main() {
    vec3 norm = normalize(fragNormal);

    // Light direction
    vec3 lightDir = normalize(lightPos - fragPos);

    // Ambient
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * fragColor;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * fragColor;

    // Specular
    float specularStrength = 0.4;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0);

    vec3 texColor = texture(texSampler, fragTexCoord).rgb;

    vec3 result = (ambient + diffuse + specular) * texColor;

    outColor = vec4(result, 1.0);
}
