#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform vec3 lightColor;

//WorldSpace
uniform vec3 lightPos;
uniform vec3 viewPos;

//View Space
//in vec3 LightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//View Space
//uniform vec3 lightPos;

out vec3 Normal;
out vec3 color;

//View Space
//out vec3 LightPos;

void main()
{
	float ambientStrength = 0.1;
	float diffuseStrength = 1.0;
	float specularStrength = 0.5;

    gl_Position = projection * view * model * vec4(aPos, 1.0);

	vec3 Position = vec3(model * vec4(aPos, 1.0));
	
	Normal = mat3(transpose(inverse(model))) * aNormal;

	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - Position);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor * diffuseStrength;

	vec3 viewDir = normalize(viewPos - Position);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	color = ambient + diffuse + specular;

	//View for viewSpace
	//FragPos = vec3(view * model * vec4(aPos, 1.0));
	//FragPos = vec3(model * vec4(aPos, 1.0));

	//Don't do this on gpu - inefficient
	//Normal = mat3(transpose(inverse(model))) * aNormal;

	//ViewSpace
	//Normal = mat3(transpose(inverse(view * model))) * aNormal;
	//LightPos = vec3(view * vec4(lightPos, 1.0));
}