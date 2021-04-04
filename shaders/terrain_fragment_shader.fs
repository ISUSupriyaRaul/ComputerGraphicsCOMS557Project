#version 410 core                                                 
//#define MAX_LIGHTS 10

uniform sampler2D terrainTex;
uniform sampler2D blendmap;
uniform sampler2D mud;	
uniform sampler2D snow;

in vec4 pass_Color;
in vec2 pass_TexCoord;
out vec4 color;   


//in vec4 transformedNormal;
//in vec4 surface_to_light;




void main(void)                                                   
{   
	// Using blend map to devide the terrain into sections based on colors and assign different textures to these sections
	vec4 blendmap_color =  texture(blendmap, pass_TexCoord.xy );         
	float bgTexcolorAmt = 1-(blendmap_color.r + blendmap_color.g + blendmap_color.b);   
	vec2 tiledCoords =  pass_TexCoord * 40.0;         
	vec4 grass_tex_color =  texture(terrainTex, tiledCoords) * bgTexcolorAmt;   ;   
	vec4 mud_tex_color =  texture(mud, tiledCoords)  * blendmap_color.g;
	vec4 snow_tex_color =  texture(snow, tiledCoords) * (blendmap_color.r + blendmap_color.b);       
	
	vec4 totalColor = mud_tex_color + grass_tex_color +	snow_tex_color ; 
	color = pass_Color * totalColor ; 
	
}                           