#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_ambient1;
uniform int which_player;

void main()
{    
	// Color Player 1 and 2 differently
	if (which_player == 1) {
		FragColor = vec4(1.0f, 0.1f, 0.1f, 1.0f);
	}

	else {
		FragColor = vec4(0.1f, 0.1f, 1.0f, 1.0f);
	}
}