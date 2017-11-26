#version 330 core
struct Material {
	vec3 diffuse;
	vec3 specular;
	float shine; 
};

uniform Material material;

uniform mat4 view;
uniform vec3 light;

in vec3 Normal;
in vec3 FragPos;

out vec4 color;

/* heavily inspired by learnopengl.com */
void main() {
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(-FragPos);
	vec3 light = mat3(view) * normalize(-vec3(1.5, -.5, -1));
	vec3 H = normalize(light + viewDir);
	float s = pow(max(0.0, dot(norm, H)), material.shine);

	vec3 result = material.diffuse * max(0.0, dot(norm, light)) + material.specular * s + vec3(.2);
  
	color = vec4(result, 1.0);
}


