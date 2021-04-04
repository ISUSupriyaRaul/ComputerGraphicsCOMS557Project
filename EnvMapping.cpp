#include "EnvMapping.h"
#include "ImgLoader.h"
#include "Shaders.h"
#include <algorithm>
#include <opencv2/opencv.hpp>

#include <cassert>
using namespace cv;
using namespace std;

CubeMap::CubeMap(){

}


CubeMap::~CubeMap()
{
	// Program clean up when the window gets closed.
	glDeleteVertexArrays(1, _vaoID);
	glDeleteProgram(_program1);
}

/*!
Init the geometry object
*/
void CubeMap::init(void)
{
	initShader();
	initVBO();
}



/*!
Create the vertex buffer object for this element
*/
void CubeMap::initVBO() // 
{
	static const GLfloat _Vertices[] = {
		-CMAPSIZE,  CMAPSIZE, -CMAPSIZE,
		-CMAPSIZE, -CMAPSIZE, -CMAPSIZE,
		CMAPSIZE, -CMAPSIZE, -CMAPSIZE,
		CMAPSIZE, -CMAPSIZE, -CMAPSIZE,
		CMAPSIZE,  CMAPSIZE, -CMAPSIZE,
		-CMAPSIZE,  CMAPSIZE, -CMAPSIZE,

		-CMAPSIZE, -CMAPSIZE,  CMAPSIZE,
		-CMAPSIZE, -CMAPSIZE, -CMAPSIZE,
		-CMAPSIZE,  CMAPSIZE, -CMAPSIZE,
		-CMAPSIZE,  CMAPSIZE, -CMAPSIZE,
		-CMAPSIZE,  CMAPSIZE,  CMAPSIZE,
		-CMAPSIZE, -CMAPSIZE,  CMAPSIZE,

		CMAPSIZE, -CMAPSIZE, -CMAPSIZE,
		CMAPSIZE, -CMAPSIZE,  CMAPSIZE,
		CMAPSIZE,  CMAPSIZE,  CMAPSIZE,
		CMAPSIZE,  CMAPSIZE,  CMAPSIZE,
		CMAPSIZE,  CMAPSIZE, -CMAPSIZE,
		CMAPSIZE, -CMAPSIZE, -CMAPSIZE,

		-CMAPSIZE, -CMAPSIZE,  CMAPSIZE,
		-CMAPSIZE,  CMAPSIZE,  CMAPSIZE,
		CMAPSIZE,  CMAPSIZE,  CMAPSIZE,
		CMAPSIZE,  CMAPSIZE,  CMAPSIZE,
		CMAPSIZE, -CMAPSIZE,  CMAPSIZE,
		-CMAPSIZE, -CMAPSIZE,  CMAPSIZE,

		-CMAPSIZE,  CMAPSIZE, -CMAPSIZE,
		CMAPSIZE,  CMAPSIZE, -CMAPSIZE,
		CMAPSIZE,  CMAPSIZE,  CMAPSIZE,
		CMAPSIZE,  CMAPSIZE,  CMAPSIZE,
		-CMAPSIZE,  CMAPSIZE,  CMAPSIZE,
		-CMAPSIZE,  CMAPSIZE, -CMAPSIZE,

		-CMAPSIZE, -CMAPSIZE, -CMAPSIZE,
		-CMAPSIZE, -CMAPSIZE,  CMAPSIZE,
		CMAPSIZE, -CMAPSIZE, -CMAPSIZE,
		CMAPSIZE, -CMAPSIZE, -CMAPSIZE,
		-CMAPSIZE, -CMAPSIZE,  CMAPSIZE,
		CMAPSIZE, -CMAPSIZE,  CMAPSIZE
	};
	

	glUseProgram(_program1);

	glGenVertexArrays(1, _vaoID); // Create our Vertex Array Object
	glBindVertexArray(_vaoID[0]); // Bind our Vertex Array Object so we can use it


	glGenBuffers(1, _vboID); // Generate our Vertex Buffer Object

	// vertices
	glBindBuffer(GL_ARRAY_BUFFER, _vboID[0]); // Bind our Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER,  sizeof(_Vertices), _Vertices, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW

	int locPos = glGetAttribLocation(_program1, "in_Position");
	glVertexAttribPointer((GLuint)locPos, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
	glEnableVertexAttribArray(locPos);

	/*Unbind Everything*/
	glBindVertexArray(0); // Disable our Vertex Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Disable our Vertex Buffer Object

	glUseProgram(0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);


}


/*
Inits the shader program for this object
*/
void CubeMap::initShader(void)
{
	if (!_apperance.exists())return;

	// This loads the shader program from a file
	_program1 = _apperance.getProgram();


	glUseProgram(_program1);

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Vertex information / names

	glBindAttribLocation(_program1, 0, "in_Position");

	_modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	addModelViewMatrixToProgram(_program1);
	glUseProgram(0);

}

/*!
Draw the objects
*/
void CubeMap::draw(void) // generateCubeMap
{

	//cam_manipulator1.setEyePosForwardDir();

	// Enable the shader program
	glUseProgram(_program1);

	// Bind the buffer and switch it to an active buffer
	glBindVertexArray(_vaoID[0]);

	// this changes the camera location
	glm::mat4 rotated_view = rotatedViewMatrix();
	glUniformMatrix4fv(_viewMatrixLocation, 1, GL_FALSE, &rotated_view[0][0]); // send the view matrix to our shader
	//glUniformMatrix4fv(_inverseViewMatrixLocation, 1, GL_FALSE, &invRotatedViewMatrix()[0][0]);
	glUniformMatrix4fv(_modelMatrixLocation, 1, GL_FALSE, &_modelMatrix[0][0]); //


	glDrawArrays(GL_TRIANGLES, 0, 108);

	// Unbind our Vertex Array Object
	glBindVertexArray(0);

	// Unbind the shader program
	glUseProgram(0);


}
