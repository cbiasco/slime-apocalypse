#version 330 core
struct Material {
	vec3 diffuse;
	vec3 specular;
	float shine; 
};

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

struct PointLight {
	vec3 position;
	
	float constant;
	float linear;
	float quadratic;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

struct SpotLight {
	vec3 position;
	vec3 direction;

	float cutOff;
	float fadeOff;
	
	float constant;
	float linear;
	float quadratic;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


#define NR_POINT_LIGHTS 4
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform Material material;

uniform mat4 view;

in vec3 Normal;
in vec3 FragPos;

out vec4 color;

/* heavily inspired by learnopengl.com */
void main() {
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(-FragPos);
	
	vec3 result;
	
	// add directional light's contributions
	result += CalcDirLight(dirLight, norm, viewDir);
	// Do the same for all point lights
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	// And add others lights as well (like spotlights)
	result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

	color = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
	// convert to view space
	vec3 lightDir = mat3(view) * light.direction;
	lightDir = normalize(-lightDir);
	// Diffuse effect
	float d = max(dot(normal, lightDir), 0.0);
	// Specular effect
	vec3 reflectDir = reflect(-lightDir, normal); // reflect by a's head at b's tail (a should point to b's tail)
	float s = pow(max(dot(viewDir, reflectDir), 0.0), material.shine);

	// Apply light
	vec3 ambient = light.ambient * material.diffuse;
	vec3 diffuse = light.diffuse * d * material.diffuse;
	vec3 specular = light.specular * s * material.specular;
	return ambient + diffuse + specular;	
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	// convert to view space	
	vec3 lightPos = vec3(view * vec4(light.position, 1.0f));
	vec3 lightDir = (lightPos - FragPos);

	// attenuation
	float distance = length(lightDir);
	lightDir = normalize(lightDir);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// diffuse effect
	float d = max(dot(normal, lightDir), 0.0);
	// specular effect
	vec3 reflectDir = reflect(-lightDir, normal); // reflect by a's head at b's tail (a should point to b's tail)
	float s = pow(max(dot(viewDir, reflectDir), 0.0), material.shine);	

	// Apply light
	vec3 ambient = light.ambient * material.diffuse;
	vec3 diffuse = light.diffuse * d * material.diffuse;
	vec3 specular = light.specular * s * material.specular;
	return (ambient + diffuse + specular) * attenuation;
}


vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	// convert to view space	
	vec3 lightPos = vec3(view * vec4(light.position, 1.0f));
	vec3 spotDir = mat3(view) * light.direction;
	vec3 lightDir = (lightPos - FragPos);

	// attenuation
	float distance = length(lightDir);
	lightDir = normalize(lightDir);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));	
	
	// spotlight effect
	float theta = dot(lightDir, normalize(-spotDir));
	float intensity = clamp((theta - light.fadeOff) / (light.cutOff - light.fadeOff), 0.0f, 1.0f);
	// diffuse effect
	float d = max(dot(normal, lightDir), 0.0);
	// specular effect
	vec3 reflectDir = reflect(-lightDir, normal); // reflect by a's head at b's tail (a should point to b's tail)
	float s = pow(max(dot(viewDir, reflectDir), 0.0), material.shine);	

	// Apply light
	vec3 ambient = light.ambient * material.diffuse;
	vec3 diffuse = light.diffuse * d * material.diffuse;
	vec3 specular = light.specular * s * material.specular;
	return (ambient + diffuse + specular) * attenuation * intensity;
}
