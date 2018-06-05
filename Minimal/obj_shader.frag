#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_ambient1;

void main()
{    
    //FragColor = texture(texture_ambient1, TexCoords);
	FragColor = vec4(Normal.r, Normal.g, Normal.b, 1.0f);
}