#version 330 core
out vec4 FragColor;

#define NR_POINT_LIGHTS 1

struct Material {
    float shininess;
};

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

struct SpotLight {
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

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform Material material;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
	vec4 FragPosSpotSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;
uniform sampler2D shadowMap2;
uniform samplerCube depthMap;

uniform vec3 viewPos;
uniform bool blinn;
uniform float far_plane;

uniform vec3 sampleOffsetDirections[20];
uniform bool pre_sampler;

float bias;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	//float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;

	if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

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

	lighting += CalcDirLight(dirLight, normal, viewDir);
	lighting += CalcPointLight(pointLights[0], normal, fs_in.FragPos, viewDir);
	lighting += CalcSpotLight(spotLight, normal, fs_in.FragPos, viewDir);

    FragColor = vec4(lighting, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){

	vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
	// ambient
    vec3 ambient = light.ambient * color;

    // diffuse 
    vec3 lightDir = normalize(light.direction - fs_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
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

	bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.025);
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, shadowMap);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

	return lighting;
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

	bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.025);
    float shadow = ShadowCalculation(fragPos);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

   return lighting;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;

	vec3 lightDir = normalize(light.position - fragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);    

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// ambient
	vec3 ambient = light.ambient * color;
    
	float diff = max(dot(normal, lightDir), 0.0);
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

	//add ambient if no light is wanted
	//ambient *= intensity;
	diffuse *= intensity;
	specular *= intensity;

	bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = ShadowCalculation(fs_in.FragPosSpotSpace, shadowMap2);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

	return lighting;
}