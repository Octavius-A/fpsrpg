#version 330 core

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    // sampler2D specular;
    // float shininess;
};

struct DirLight {   // Directional light e.g the sun
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
};
uniform DirLight dirLight;


in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;

vec4 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // calculate directional light
    vec4 result = calcDirLight(dirLight, norm, viewDir);

    FragColor = result;
}

vec4 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    // ambient
    vec4 tx = texture(material.diffuse, TexCoords).rgba;
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
            
    vec3 result = ambient + diffuse;
    vec4 final = vec4(result, tx.a);
    return final;
}