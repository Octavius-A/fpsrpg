#version 330 core

in vec2 FragPos;

out vec4 color;

uniform sampler2D sprite;

//uniform float life;

void main() {
    color = texture(sprite, FragPos).rgba;
}