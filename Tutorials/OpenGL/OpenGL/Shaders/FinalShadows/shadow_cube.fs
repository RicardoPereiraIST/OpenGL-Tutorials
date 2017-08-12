#version 330 core
out vec4 FragColor;

#define NR_POINT_LIGHTS 1

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

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
};

uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

uniform vec3 viewPos;

uniform bool blinn;
uniform float far_plane;

uniform vec3 sampleOffsetDirections[20];
uniform bool pre_sampler;

float bias;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - pointLights[0].position;
    // ise the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    //float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    //float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
     
	
	//With static sample number
	float shadow = 0.0;

	if(pre_sampler){
		int samples  = 20;
		float viewDistance = length(viewPos - fragPos);
		float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;  
		for(int i = 0; i < samples; ++i)
		{
			float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
			closestDepth *= far_plane;   // Undo mapping [0;1]
			if(currentDepth - bias > closestDepth)
				shadow += 1.0;
		}
		shadow /= float(samples);
	}

	else{
		float samples = 4.0;
		float offset  = 0.1;
		for(float x = -offset; x < offset; x += offset / (samples * 0.5))
		{
			for(float y = -offset; y < offset; y += offset / (samples * 0.5))
			{
				for(float z = -offset; z < offset; z += offset / (samples * 0.5))
				{
					float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; 
					closestDepth *= far_plane;   // Undo mapping [0;1]
					if(currentDepth - bias > closestDepth)
						shadow += 1.0;
				}
			}
		}
		shadow /= (samples * samples * samples);
	}
	    
    return shadow;
}

void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);

	vec3 lighting = vec3(0.0);

	lighting = CalcPointLight(pointLights[0], normal, fs_in.FragPos, viewDir);

    FragColor = vec4(lighting, 1.0);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// ambient
    vec3 ambient = light.ambient * color;

    // diffuse 
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = light.diffuse * (diff);
    
    // specular

	float spec = 0.0;

	if(blinn)
	{
		vec3 halfwayDir = normalize(lightDir + viewDir);  
		spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	}
	else
	{
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	}

	vec3 specular = light.specular * (spec);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = ShadowCalculation(fragPos);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

   return lighting;
}