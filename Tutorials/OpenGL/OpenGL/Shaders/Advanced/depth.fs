#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

float near = 0.1; 
float far  = 100.0;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


void main()
{
	//DEPTH

	float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration

    //Normal z buffer (NON LINEAR) -> white afar, black close
	//FragColor = vec4(vec3(gl_FragCoord.z),1.0);

	//LINEAR
	//FragColor = vec4(vec3(depth), 1.0);

	//Textures

	//BLEND
	vec4 texColor = texture(texture1, TexCoord);
	/*
	if(texColor.a < 0.1)
        discard;
	*/
	FragColor = texColor;
}