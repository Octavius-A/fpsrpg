#version 330 core

layout(location = 0) in vec3 vertices;
// layout(location = 1) in vec2 texCoords;

out vec2 FragPos;

uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 position;
uniform float scale;

void main() {
    vec3 particleCenter_worldspace = position;

    vec3 vertexPosition_worldspace = particleCenter_worldspace
        + CameraRight_worldspace * vertices.x * scale
        + CameraUp_worldspace * vertices.y * scale;
    
    gl_Position = projection * view * vec4(vertexPosition_worldspace, 1.0f);
    FragPos = vertices.xy + vec2(0.5f, 0.5f);
}