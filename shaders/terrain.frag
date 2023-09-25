#version 330 core

uniform sampler2D u_texture_0;
uniform vec4 u_color;
uniform float u_time;
uniform vec3 u_view_pos;

out vec4 fragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

const float exposure = 1.0;

#include voro.glsl

#include simplex2d.glsl

void main()
{
    float n = snoise(texCoord * 12.0) * 0.5 + 0.5 + 0.4;
    float noise = fbm(texCoord * 512.0) * 1.4 * 0.5 + 0.5;//texture(u_texture_0, texCoord).rgb;

    fragColor = vec4(mix(#f4a460, #a0522d, noise * n), 1.0);
    fragColor.rgb = fragColor.rgb * 0.2 + fragColor.rgb * 0.6 * vec3(0.9, 0.4, 0.2);
    fragColor.rgb = mix(mix(#87CEFA, vec3(0.9, 0.4, 0.2) * 3.0, snoise(texCoord * 128.0) * 0.3 + 0.3), fragColor.rgb, min(1.0, n));
    fragColor.rgb = vec3(1.0) - exp(-fragColor.rgb * exposure);
    fragColor.rgb = pow(fragColor.rgb, vec3(1.0/2.2));
}