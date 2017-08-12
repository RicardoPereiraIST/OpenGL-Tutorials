version 330 core

out vec4 FragColor;

#define NR_POINT_LIGHTS 4

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

uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform sampler2D texture_reflection1;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 Position, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 Position, vec3 viewDir);

void main()
{   
	// properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(cameraPos - Position);
	vec3 result = vec3(0.0);
    // phase 1: Directional lighting
    result += CalcDirLight(dirLight, norm, viewDir);
    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, Position, viewDir);    
    // phase 3: Spot light
    result += CalcSpotLight(spotLight, norm, Position, viewDir);


    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
	vec4 texColor = texture(texture_reflection1, TexCoords);
	vec4 skyboxColor = texture(skybox, R);
	FragColor = mix(skyboxColor, vec4(result, 1.0), 0.5);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
	// ambient
    vec3 ambient = light.ambient * vec3(texture(texture_reflection1, TexCoords));

    // diffuse 
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(texture_reflection1, TexCoords)));
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(texture_reflection1, TexCoords)));

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 Position, vec3 viewDir)
{
	float distance = length(light.position - Position);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// ambient
    vec3 ambient = light.ambient * vec3(texture(texture_reflection1, TexCoords));

    // diffuse 
    vec3 lightDir = normalize(light.position - Position);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(texture_reflection1, TexCoords)));
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(texture_reflection1, TexCoords)));  

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

   return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 Position, vec3 viewDir){
	vec3 lightDir = normalize(light.position - Position);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);    

	float distance = length(light.position - Position);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// ambient
	vec3 ambient = light.ambient * vec3(texture(texture_reflection1, TexCoords));
    
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * vec3(texture(texture_reflection1, TexCoords)));
    
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * vec3(texture(texture_reflection1, TexCoords)));  

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	//add ambient if no light is wanted
	//ambient *= intensity;
	diffuse *= intensity;
	specular *= intensity;

	return (ambient + diffuse + specular);
}