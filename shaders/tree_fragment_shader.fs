#version 410 core                                                 

uniform sampler2D treeTexture;
in vec4 pass_Color;
in vec2 pass_TexCoord;
out vec4 color;   

void main(void)                                                   
{   
	vec4 tex_color =  texture(treeTexture, pass_TexCoord.xy);    
	color =  pass_Color * tex_color;	
}                           