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


struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};
#define MAX_POINT_LIGHTS 10
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;

vec4 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec4 calcPointLight(PointLight light, vec3 normal, vec3 viewDir);

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
 
    // calculate point light
    vec4 result = vec4(0.0f);
    for (int i = 0; i < pointLightCount; i++) {
        result += calcPointLight(pointLights[i], norm, viewDir);
    }

    FragColor = result;
}

vec4 calcPointLight(PointLight light, vec3 normal, vec3 viewDir) {
    vec4 tx = texture(material.diffuse, TexCoords).rgba;
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambient  *= attenuation;  
    diffuse   *= attenuation;
        
    vec3 result = ambient + diffuse;
    vec4 final = vec4(result, tx.a);
    return final;
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