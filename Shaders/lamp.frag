#version 330 core

out vec4 color;

uniform vec3 lightColor;

/* heavily inspired by learnopengl.com */
void main() {
	color = vec4(lightColor, 1.0f);
}
