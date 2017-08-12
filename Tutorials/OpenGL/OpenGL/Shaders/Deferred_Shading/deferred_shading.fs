#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Material {
    float shininess;
};

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float radius;
};

const int NR_LIGHTS = 32;
uniform PointLight pointLights[NR_LIGHTS];
uniform Material material;
uniform vec3 viewPos;

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    // then calculate lighting as usual
    vec3 lighting = vec3(0.0); // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);

    for(int i = 0; i < NR_LIGHTS; ++i)
    {
		// calculate distance between light source and current fragment
		//NOT ACTUALLY DOING ANYTHING BECAUSE GPU AND GLSL CAN'T OPTIMIZE BRANCHES - IT'S ONLY THE IDEA BEHIND
		//SOLUTION IS TO CREATE SPHERE SCALED TO RADIUS AND ONLY RENDER THEM WITH THIS SHADER
        float distance = length(pointLights[i].position - FragPos);
        if(distance < pointLights[i].radius)
        {
			lighting += Diffuse * pointLights[i].diffuse;
			
			// diffuse
			vec3 lightDir = normalize(pointLights[i].position - FragPos);
			vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * pointLights[i].specular;
			// specular
			vec3 halfwayDir = normalize(lightDir + viewDir);  
			float spec = pow(max(dot(Normal, halfwayDir), 0.0), material.shininess);
			vec3 specular = pointLights[i].specular * spec * Specular;
			// attenuation
			float distance = length(pointLights[i].position - FragPos);
			float attenuation = 1.0 / (pointLights[i].constant + pointLights[i].linear * distance + pointLights[i].quadratic * distance * distance);
			diffuse *= attenuation;
			specular *= attenuation;
			lighting += diffuse + specular;
		}
    }
    FragColor = vec4(lighting, 1.0);
}