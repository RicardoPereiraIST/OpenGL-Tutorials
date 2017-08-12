#version 330 core
out vec4 FragColor;

in VS_OUT {
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
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D depthMap;

uniform float height_scale;
uniform bool steep;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main()
{           
	vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

	vec2 texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir);

	if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
		discard;

     // obtain normal from normal map in range [0,1]
    vec3 normal = texture(texture_normal1, texCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
   
    // get diffuse color
    vec3 color = texture(texture_diffuse1, texCoords).rgb;
    // ambient
    vec3 ambient = fs_in.ambient * color;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color * fs_in.diffuse;
    // specular
    
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = fs_in.specular * spec;

	float distance = length(fs_in.TangentLightPos - fs_in.TangentFragPos);
	float attenuation = 1.0 / (fs_in.constant + fs_in.linear * distance + fs_in.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir){
	if(steep){
		const float minLayers = 8.0;
		const float maxLayers = 32.0;
		float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  

		// calculate the size of each layer
		float layerDepth = 1.0 / numLayers;
		// depth of current layer
		float currentLayerDepth = 0.0;
		// the amount to shift the texture coordinates per layer (from vector P)
		vec2 P = viewDir.xy * height_scale; 
		vec2 deltaTexCoords = P / numLayers;

		// get initial values
		vec2  currentTexCoords     = texCoords;
		float currentDepthMapValue = texture(depthMap, currentTexCoords).r;
  
		while(currentLayerDepth < currentDepthMapValue)
		{
			// shift texture coordinates along direction of P
			currentTexCoords -= deltaTexCoords;
			// get depthmap value at current texture coordinates
			currentDepthMapValue = texture(depthMap, currentTexCoords).r;  
			// get depth of next layer
			currentLayerDepth += layerDepth;  
		}

		//OCLUSION

		// get texture coordinates before collision (reverse operations)
		vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

		// get depth after and before collision for linear interpolation
		float afterDepth  = currentDepthMapValue - currentLayerDepth;
		float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
		// interpolation of texture coordinates
		float weight = afterDepth / (afterDepth - beforeDepth);
		vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

		return finalTexCoords;  
	}
	else{
		float height =  texture(depthMap, texCoords).r;    
		vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
		return texCoords - p;
	}
}