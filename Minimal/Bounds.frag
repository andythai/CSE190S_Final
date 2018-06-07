#version 330 core

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

uniform vec3 collision_color;

void main()
{
    // Color everything a hot pink color. An alpha of 1.0f means it is not transparent.
    color = vec4(collision_color.r, collision_color.g, 0.0f, 1.0f);
}