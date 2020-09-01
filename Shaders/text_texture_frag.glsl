#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

const float width = 0.55;
const float edge = 0.2;

const float borderWidth = 0.7;
const float borderEdge  = 0.1;

const vec3  outlineColor = vec3(1.0, 0.0, 0.0);

const vec2 offset = vec2(0.003, 0.003);

void main()
{

    float distance = 1.0 - texture2D(ourTexture, TexCoord).a;
    float alpha = 1.0 - smoothstep(width, width+edge, distance);


    float distance2 = 1.0 - texture(ourTexture, TexCoord + offset).a;
    float outlineAlpha = 1.0 - smoothstep(borderWidth, borderWidth + borderEdge, distance2);

    float overallAlpha = alpha + ( 1.0 - alpha) * outlineAlpha;
    vec3 overallColor = mix(outlineColor, ourColor, alpha / overallAlpha);

    FragColor = vec4(overallColor, overallAlpha);
}
