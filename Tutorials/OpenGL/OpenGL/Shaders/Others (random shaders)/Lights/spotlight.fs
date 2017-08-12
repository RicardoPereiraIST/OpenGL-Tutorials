#version 330 core

struct Material {
	sampler2D ambient;
	sampler2D diffuse;
    sampler2D specular;
	sampler2D emission;
    float shininess;
};

struct Light {
    vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
    float linear;
    float quadratic;
};

in vec2 TexCoords;

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

void main(){
	vec3 lightDir = normalize(light.position - FragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);    

	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// ambient
	vec3 ambient = light.ambient * vec3(texture(material.ambient, TexCoords));

	// diffuse 
	vec3 norm = normalize(Normal);
    
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));
    
	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));  
    
	//Not initializing gives colors do box, which is awesome
	vec3 emission = vec3(0.0);
	if (texture(material.specular, TexCoords).r == 0.0)   /*rough check for blackbox inside spec texture */
	{
		/*apply emission texture */
		emission = vec3(texture(material.emission, TexCoords));
	}

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	diffuse *= intensity;
	specular *= intensity;

	vec3 result = ambient + diffuse + specular + emission;

	FragColor = vec4(result, 1.0);
}