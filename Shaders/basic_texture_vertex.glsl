#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 transformation_mat;

void main()
{
#if 1
    gl_Position = transformation_mat * vec4(aPos, 1.0);
#else
    gl_Position = vec4(aPos,1.0);
#endif
    ourColor = aColor;
    TexCoord = aTexCoord;
}

