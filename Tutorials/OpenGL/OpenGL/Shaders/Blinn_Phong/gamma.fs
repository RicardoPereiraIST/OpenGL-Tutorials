#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
	vec3 color = texture(screenTexture, TexCoords).rgb;
	vec3 result = pow(color, vec3(1/2.2));
	FragColor = vec4(result, 1.0);
}