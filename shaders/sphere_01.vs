#version 330 core
//#version 410 core                                                 
#define MAX_LIGHTS 10

// The vertex buffer input
in vec3 in_Color;
in vec3 in_Position;
in vec3 in_Normal;

// Transformations for the projections
uniform mat4 projectionMatrixBox;
uniform mat4 viewMatrixBox;
uniform mat4 modelMatrixBox;
uniform mat4 inverseViewMatrix;

// variable to distinguish between renderer for the selection buffer and the regular renderer
uniform bool select_mode;
uniform vec4 select_color_id;
uniform bool is_selected; // to indicate that this object has been selected

// the color that shows up when an object has been picked
const vec3 select_color = vec3(1.0,0.0,0.0);

uniform int numLights;
 
// The light sources
uniform struct Light {
    vec4 light_position;
    float diffuse_intensity;
    float ambient_intensity;
    float specular_intensity;
    float attenuationCoefficient;
    float cone_angle;
    vec3 cone_direction;
} allLights[MAX_LIGHTS];


// The material parameters
uniform struct Material {
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
    vec3 emissive;
    float shininess;
    float transparency;
} allMaterials[1];

// The output color
out vec4 pass_Color;

vec4 useLight(Light light, vec4 surfacePostion, vec4 normal_transformed, vec3 normal, Material material )
{
    vec4 surface_to_light ; 
	float attenuation;

    if(light.light_position.w == 0.0) 
	{	// Directional Light and Point Light
		surface_to_light =   normalize( light.light_position);
		// We check the angle of our light
		float light_to_surface_angle = dot(surface_to_light, normal_transformed);
		// Check the value, if the value is greater than 0.0,
		if(light_to_surface_angle > 0.0) attenuation = 1.0;
		else attenuation = 0.0;
    }
	else 
	{ 	//Spot light
		surface_to_light =   normalize(light.light_position - surfacePostion); ////
		float distanceToLight = length(light.light_position.xyz - surfacePostion.xyz);
		attenuation = 1.0 / (1.0 + light.attenuationCoefficient * pow(distanceToLight,2)); 
	    // ray direction
        vec3 ray_direction = -surface_to_light.xyz;
		
        // Normalize the cone direction
        vec3 cone_direction_normal = normalize(light.cone_direction);
    
        // Angle between light and surface 
        float light_to_surface_angle = degrees(acos(dot(ray_direction, cone_direction_normal))) ;
    
        // Compare this angle with cone angle and eliminate attenuation if the light falls outside the cone angle
        if(light_to_surface_angle > light.cone_angle)
		{
            attenuation = 0.0; // For Sharp Spotlight edges
		}

	}
	
	// ambient color
	vec3 out_ambient_color = material.ambient * light.ambient_intensity;
	
	// Diffuse color
	float diffuse_coefficient = max( dot(normal_transformed, surface_to_light), 0.0); ////
    vec3 out_diffuse_color = material.diffuse * diffuse_coefficient * light.diffuse_intensity; 

    // Specular color
    vec3 incidenceVector = -surface_to_light.xyz;												 
	
	// Calualte the reflection
    vec3 reflectionVector = reflect(incidenceVector, normal_transformed.xyz); 				

	// Determine the camera position. Note, the inverse view matrix describes the position using the camera as origin. 
    vec3 cameraPosition = vec3( inverseViewMatrix[3][0], inverseViewMatrix[3][1], inverseViewMatrix[3][2]);
    
	// CHECKE WHETHER THE DIRECTION ALIGNS WITH YOUR CALCULATION
	vec3 surfaceToCamera = normalize(cameraPosition - surfacePostion.xyz);
	
	// cosine angle between the reflection vector and the eye vector. 
    float cosAngle = max( dot(surfaceToCamera, reflectionVector), 0.0);
	
	//Calclate the specular coefficient
    float specular_coefficient = pow(cosAngle, material.shininess);
	
	//create a specular material. 
    vec3 out_specular_color = material.specular * specular_coefficient * light.specular_intensity;
    
   
    // final color using a linear color model. 
    vec3 linearColor = out_ambient_color + attenuation * ( out_diffuse_color + out_specular_color);
    
    // return the value
    return vec4(linearColor, material.transparency); 
}

void main(void)
{
    // Caculate the normal vector and surface position in world coordinates
    vec3 normal = normalize(in_Normal);
    vec4 transformedNormal =  transpose(inverse(modelMatrixBox)) * vec4( normal, 1.0 );
    vec4 surfacePostion = modelMatrixBox * vec4(in_Position, 1.0);  
 
     // Calculate the color
     vec4 linearColor = vec4(0.0,0.0,0.0,0.0);
    
    for (int i=0; i<numLights; i++) {
        vec4 new_light = useLight(allLights[i], surfacePostion, transformedNormal, normal, allMaterials[0] );
        //linearColor = linearColor + new_light;
		linearColor = mix(linearColor,new_light,0.5); // used 0.5 to interpolate between color and light
    }
       
    // Gamma correction
    vec4 gamma = vec4(1.0/2.2);
    vec4 finalColor = pow(linearColor, gamma);
    
    // Pass the color
    pass_Color =  vec4(finalColor);
 
    // Passes the projected position to the fragment shader / rasterization process.
    gl_Position = projectionMatrixBox * viewMatrixBox * modelMatrixBox * vec4(in_Position, 1.0);
    
}






















