#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 incolor;
out vec2 uvtex;

void main()
{
    gl_Position = vec4(aPos,1.0);

    incolor = aColor;
    uvtex = aTexCoord;
}
