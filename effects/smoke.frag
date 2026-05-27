#version 330 core

in vec4 color;
in vec2 UV;

uniform sampler2D smokeTex;

out vec4 FragColor;

void main()
{
    vec4 tex = texture(smokeTex, UV);

    float d = tex.r;

    // sharper structure
    d = smoothstep(0.2, 1.0, d);

    // texture affects brightness
    vec3 finalColor = color.rgb * d;

    float alpha = d * color.a;

    FragColor = vec4(finalColor, alpha);

    if(alpha < 0.01)
        discard;
}