//#version 330 core
#version 410 core                                                 

in vec3 in_Position;
out vec3 textureCoords;

uniform mat4 projectionMatrixBox;
uniform mat4 viewMatrixBox;
uniform mat4 modelMatrixBox;

void main(void)
{
     
    // Pass the texture Coordinates
    textureCoords =  in_Position;
 
    // Passes the projected position to the fragment shader / rasterization process.
    gl_Position = projectionMatrixBox * viewMatrixBox * modelMatrixBox * vec4(in_Position, 1.0);
    
}






















