#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    // sampler2D specular;
    // float shininess;
};

uniform samplerCube depthMap;

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
uniform float far_plane;

vec3 gridSamplingDisk[20] = vec3[]
(
    vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
    vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
    vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
    vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
    vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);


vec4 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec4 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, float shadow);
float calcShadow(PointLight light, vec3 fragPos);

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
 
    float shadow = calcShadow(pointLights[0], FragPos);

    // calculate point light
    vec4 result = vec4(0.0f);
    for (int i = 0; i < pointLightCount; i++) {
        result += calcPointLight(pointLights[i], norm, viewDir, shadow);
    }

    FragColor = result;
}

vec4 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, float shadow) {
    vec4 tx = texture(material.diffuse, TexCoords).rgba;
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambient  *= attenuation;  
    diffuse   *= attenuation;
        
    vec3 result = (ambient + (1.0 - shadow)) + diffuse;
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

float calcShadow(PointLight light, vec3 fragPos) {
    vec3 fragToLight = fragPos - light.position;
    float closestDepth = texture(depthMap, fragToLight).r;
    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);
    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    // vec3 fragToLight = fragPos - light.position;

    // float currentDepth = length(fragToLight);
    
    // // experiment with vals
    // float shadow = 0.0;
    // float bias = 0.15;
    
    
    //int samples = 20;
    // float viewDistance = length(viewPos - fragPos);
    // float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    // for (int i = 0; i < samples; ++i) {
    //     float closestDepth = texture(depthMap, fragToLight, gridSamplingDisk[i] * diskRadius).r;
    //     closestDepth *= far_plane;
    //     if (currentDepth - bias > closestDepth) {
    //         shadow += 1.0;
    //     }
    // }
    // shadow /= float(samples);

    return shadow;
}