#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//View Space
//uniform vec3 lightPos;

out vec3 Normal;
out vec3 FragPos;

//View Space
//out vec3 LightPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

	//View for viewSpace
	//FragPos = vec3(view * model * vec4(aPos, 1.0));
	FragPos = vec3(model * vec4(aPos, 1.0));

	//Don't do this on gpu - inefficient
	Normal = mat3(transpose(inverse(model))) * aNormal;

	//ViewSpace
	//Normal = mat3(transpose(inverse(view * model))) * aNormal;
	//LightPos = vec3(view * vec4(lightPos, 1.0));
}