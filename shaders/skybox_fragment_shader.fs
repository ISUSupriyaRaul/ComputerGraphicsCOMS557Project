#version 410 core                                                 

uniform samplerCube skyBox; // Cube map
in vec3 textureCoords;
out vec4 color;   

void main(void)                                                   
{   
	color =  texture(skyBox, textureCoords);    
}                           