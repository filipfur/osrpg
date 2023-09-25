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

void main()
{
    vec3 lightDir = normalize(vec3(1.0, 5.0, 1.0) - fragPos);

    vec3 color = texture(u_texture_0, texCoord).rgb;

    vec3 ambient = 0.4 * color;

    vec3 diffuse = max(dot(normal, lightDir), 0.0) * color;

    float spec = pow(max(dot(normalize(u_view_pos - fragPos), reflect(-lightDir, normal)), 0.0), 128.0);

    vec3 lightning = ambient + (diffuse + spec * 0.5) * 0.5;

    fragColor = vec4(lightning, 1.0);
    fragColor.rgb = vec3(1.0) - exp(-fragColor.rgb * exposure);
    fragColor.rgb = pow(fragColor.rgb, vec3(1.0/2.2));
}