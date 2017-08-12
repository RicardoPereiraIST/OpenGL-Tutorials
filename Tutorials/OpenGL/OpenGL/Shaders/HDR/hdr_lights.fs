#version 330 core
out vec4 FragColor;

#define NR_POINT_LIGHTS 4

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

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
uniform sampler2D diffuseTexture;
uniform vec3 viewPos;

void main()
{
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);

	vec3 ambient = vec3(0.0);

    // lighting
    vec3 lighting = vec3(0.0);
	vec3 specular = vec3(0.0);

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
		// ambient
		ambient += pointLights[i].ambient * color;

        // diffuse
        vec3 lightDir = normalize(pointLights[i].position - fs_in.FragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = pointLights[i].diffuse * diff * color;
        vec3 result = diffuse;        
        // attenuation (use quadratic as we have gamma correction)
		float distance = length(fs_in.FragPos - pointLights[i].position);
		float attenuation = 1.0 / ( pointLights[i].constant +  pointLights[i].linear * distance +  pointLights[i].quadratic * (distance * distance));
        result *= attenuation;
		lighting += result;

		//spec
		vec3 reflectDir = reflect(-lightDir, normal);
		vec3 halfwayDir = normalize(lightDir + viewDir);  
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

		
		specular += pointLights[i].specular * spec;

		ambient *= attenuation;
		specular *= attenuation;
                
    }
    FragColor = vec4(ambient + lighting + specular, 1.0);
}