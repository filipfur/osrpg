#version 330 core

uniform sampler2D u_texture_0;
uniform vec4 u_color;
uniform float u_time;
uniform vec3 u_view_pos;

out vec4 fragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

const float exposure = 5.0;

void main()
{
    
    vec3 color = texture(u_texture_0, texCoord).rgb;

    fragColor = vec4(color, 1.0);
    fragColor.rgb = vec3(1.0) - exp(-fragColor.rgb * exposure);
    fragColor.rgb = pow(fragColor.rgb, vec3(1.0/2.2));
}