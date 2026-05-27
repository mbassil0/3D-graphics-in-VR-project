#version 330 core

in vec3 vertex;
in vec4 center;
in vec4 col;

uniform vec3 cameraRight;
uniform vec3 cameraUp;

uniform mat4 V;
uniform mat4 P;

out vec4 color;
out vec2 UV;

void main()
{
    float scale = center.w;

    vec3 particleCenter = center.xyz;

    vec3 vertexPosition_worldspace =
        particleCenter
        + cameraRight * vertex.x * scale
        + cameraUp * vertex.y * scale;

    gl_Position =
        P * V * vec4(vertexPosition_worldspace, 1.0);

    color = col;

    // map [-1,1] quad coords -> [0,1] UV coords
    UV = vertex.xy * 0.5 + 0.5;
}