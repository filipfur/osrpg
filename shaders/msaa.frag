#version 330 core

out vec4 fragColor;

in vec2 texCoord;
uniform sampler2DMS u_texture;
uniform sampler2DMS u_depth_map;
uniform ivec2 u_resolution;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    vec4 diffuse = vec4(0.0);
    float depth = 0.0;
    for(int i=0; i < 4; i++)
    {
        diffuse += texelFetch(u_texture, ivec2(texCoord * u_resolution), i);
        depth += texelFetch(u_depth_map, ivec2(texCoord * u_resolution), i).r;
    }
    diffuse *= 0.25;
    depth *= 0.25;

    depth = LinearizeDepth(depth) / far;

    float vingette = 1.0 - pow(length(texCoord - vec2(0.5)), 2.0);

    fragColor = diffuse * (1.0 - min(depth, 0.8)) * vingette;
}