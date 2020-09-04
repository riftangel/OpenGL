#version 330 core

in vec2 uvtex;
in vec3 incolor;

layout(location = 0) out vec3 color;

uniform sampler2D quadtex;

void main() {

    color = texture(quadtex, uvtex).rgb;
}
