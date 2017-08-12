#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

#define NR_POINT_LIGHTS 1

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;

	float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} vs_out;

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

//uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool calculateOwnBit;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));   
    vs_out.TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);

	mat3 TBN;

	if(calculateOwnBit){
		T = normalize(T - dot(T, N) * N);
		vec3 B = cross(N, T);
		TBN = transpose(mat3(T, B, N));	
	}
	else{
		vec3 B = normalize(normalMatrix * aBitangent);

		if (dot(cross(N, T), B) < 0.0)
					T = T * -1.0;
		TBN = mat3(T, B, N);
	}
    
    vs_out.TangentLightPos = TBN * pointLights[0].position;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;

	vs_out.constant = pointLights[0].constant;
	vs_out.linear = pointLights[0].linear;
	vs_out.quadratic = pointLights[0].quadratic;
	vs_out.ambient = pointLights[0].ambient;
	vs_out.diffuse = pointLights[0].diffuse;
	vs_out.specular = pointLights[0].specular;
        
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}