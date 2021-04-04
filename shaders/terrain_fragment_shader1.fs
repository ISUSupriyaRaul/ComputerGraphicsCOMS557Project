#version 410 core                                                 

uniform samplerCube CubeMap; // cube map texture    
uniform sampler2D RiverMap; // color map texture    
uniform sampler2D noiseMap; // normal map texture 
uniform float time;
in vec4 pass_Color;
in vec2 pass_TexCoord;
out vec4 color;   

in vec3 pass_normal;
in vec3 pass_viewposition;

void main(void)                                                   
{   
	
	//-------------- Displacement mapping: -------------- Ripples in the river
	// addding time factor in the texture coordinates
	vec2 noiseVec = vec2( pass_TexCoord.x + time, pass_TexCoord.y + time); 
	// using the noise vector to calculate the noise texture color and displacement
	vec4 noise = texture ( noiseMap , noiseVec );
	float displace =  noise.b * 5.0;
	vec2 uvcoorddisplaced =  vec2( pass_TexCoord.x + displace, pass_TexCoord.y + displace);
	// final color calculation		
	vec4 rivercolor = texture ( RiverMap , uvcoorddisplaced );
	
	//-------------- Environment mapping: --------------  Reflection of the sky in the river
	vec3 reflection = reflect(pass_viewposition, normalize(pass_normal));
	//vec3 r = vec3(reflection.x, -reflection.yz); // only needed if we do vec3 surfaceToCamera = normalize(cameraPosition - surfacePostion.xyz) in vertex shader
	vec4 cubemap_color = texture(CubeMap, reflection);
	
	//color = cubemap_color;
	color = 0.3 * rivercolor * pass_Color + cubemap_color;
	
} 
 