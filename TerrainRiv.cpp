#include "TerrainRiv.h"
#include "ImgLoader.h"
#include "Shaders.h"
#include <algorithm>

using namespace std;



TerrainRiv::TerrainRiv(int gridX, int gridY) :
	_x(gridX*SIZE), _z(gridY*SIZE) 
{

}


TerrainRiv::~TerrainRiv()
{
	// Program clean up when the window gets closed.
	glDeleteVertexArrays(1, _vaoID);
	glDeleteProgram(_program1);
}

/*!
Init the geometry object
*/
void TerrainRiv::init(void)
{
	initShader();
	initVBO();
}


/*!
Create the vertex buffer object for this element
*/
void TerrainRiv::initVBO() // const char* HeightMap
{
	
	int count = VERTEX_COUNT * VERTEX_COUNT;
	int countI = (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1);
	float* vertices = new float[count * 3]; // int *vertices = malloc(count * 3 * sizeof(float)); float* vertices = new float[30];
	float* colors = new float[count * 3];
	float* normals = new float[count * 3]; //int *normals = malloc(count * 3 * sizeof(float));
	float* textureCoords = new float[count * 2]; //int *textureCoords = malloc(count * 2 * sizeof(float));
	int* indices = new int[6 * (VERTEX_COUNT - 1)*(VERTEX_COUNT - 1)]; //int *indices = malloc(6 * (VERTEX_COUNT - 1)*(VERTEX_COUNT - 1) * sizeof(int));
	int vertexPointer = 0;
	for (int i = 0; i<VERTEX_COUNT; i++) {
		for (int j = 0; j<VERTEX_COUNT; j++) {
			vertices[vertexPointer * 3] = (float)j / ((float)VERTEX_COUNT - 1) * SIZE;
			vertices[vertexPointer * 3 + 1] = -30;
			vertices[vertexPointer * 3 + 2] = (float)i / ((float)VERTEX_COUNT - 1) * SIZE;
			normals[vertexPointer * 3] = 0;
			normals[vertexPointer * 3 + 1] = 1;
			normals[vertexPointer * 3 + 2] = 0;
			colors[vertexPointer * 3] = 0.0;
			colors[vertexPointer * 3 + 1] = 0.0;
			colors[vertexPointer * 3 + 2] = 1.0;
			textureCoords[vertexPointer * 2] = (float)j / ((float)VERTEX_COUNT - 1);
			textureCoords[vertexPointer * 2 + 1] = (float)i / ((float)VERTEX_COUNT - 1);
			vertexPointer++;
		}
	}
	int pointer = 0;
	for (int gz = 0; gz<VERTEX_COUNT - 1; gz++) {
		for (int gx = 0; gx<VERTEX_COUNT - 1; gx++) {
			int topLeft = (gz*VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1)*VERTEX_COUNT) + gx;
			int bottomRight = bottomLeft + 1;
			indices[pointer++] = topLeft;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topRight;
			indices[pointer++] = topRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = bottomRight;
		}
	}


	GLuint elementIDTS;

	glUseProgram(_program1);

	glGenVertexArrays(1, _vaoID); // Create our Vertex Array Object
	glBindVertexArray(_vaoID[0]); // Bind our Vertex Array Object so we can use it


	glGenBuffers(4, _vboID); // Generate our Vertex Buffer Object

							 // vertices
	glBindBuffer(GL_ARRAY_BUFFER, _vboID[0]); // Bind our Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), vertices, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW

	int locPos = glGetAttribLocation(_program1, "in_Position");
	glVertexAttribPointer((GLuint)locPos, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
	glEnableVertexAttribArray(locPos);

	/* Create the Element array Buffer Object*/ // Generate our Index Buffer Object
	glGenBuffers(1, &elementIDTS);  // Generate our Index Buffer Object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementIDTS);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, countI * 6 * sizeof(GLint), indices, GL_STATIC_DRAW);

	// normal
	glBindBuffer(GL_ARRAY_BUFFER, _vboID[1]); // Bind our second Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), normals, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW

	int locNorm = glGetAttribLocation(_program1, "in_Normal");
	glVertexAttribPointer((GLuint)locNorm, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
	glEnableVertexAttribArray(locNorm);

	// texture
	glBindBuffer(GL_ARRAY_BUFFER, _vboID[2]); // Bind our second Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, count * 2 * sizeof(float), textureCoords, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW

	int logTexCoord = glGetAttribLocation(_program1, "in_Texcoords");
	glVertexAttribPointer((GLuint)logTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
	glEnableVertexAttribArray(logTexCoord);

	//Color
	glBindBuffer(GL_ARRAY_BUFFER, _vboID[3]); // Bind our second Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, count * 3 * sizeof(float), colors, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW

	int logColor = glGetAttribLocation(_program1, "in_Color");
	glVertexAttribPointer((GLuint)logColor, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
	glEnableVertexAttribArray(logColor);

	/*Unbind Everything*/
	glBindVertexArray(0); // Disable our Vertex Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Disable our Vertex Buffer Object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Disable our Vertex Buffer Object

	delete[] vertices; //free(vertices);
	delete[] indices; // free(indices);
	delete[] normals; // free(normals);
	delete[] textureCoords; // free(textureCoords);
	delete[] colors; // free(colors);


	glUseProgram(0);


}


/*
Inits the shader program for this object
*/
void TerrainRiv::initShader(void)
{
	if (!_apperance.exists())return;

	// This loads the shader program from a file
	_program1 = _apperance.getProgram();


	glUseProgram(_program1);

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Vertex information / names

	glBindAttribLocation(_program1, 0, "in_Position");
	glBindAttribLocation(_program1, 1, "in_Normal");
	glBindAttribLocation(_program1, 2, "in_Texcoords");
	glBindAttribLocation(_program1, 3, "in_Color");

	_modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(_x, 0.0f, _z));
	addModelViewMatrixToProgram(_program1);
	glUseProgram(0);


}

/*!
Draw the objects
*/
void TerrainRiv::draw(void) // generateTerrain
{
	static float time = 0.0;
	time += 0.0001;

	// Enable the shader program
	glUseProgram(_program1);


	// this changes the camera location
	glm::mat4 rotated_view = rotatedViewMatrix();
	glUniformMatrix4fv(_viewMatrixLocation, 1, GL_FALSE, &rotated_view[0][0]); // send the view matrix to our shader
	glUniformMatrix4fv(_inverseViewMatrixLocation, 1, GL_FALSE, &invRotatedViewMatrix()[0][0]);
	glUniformMatrix4fv(_modelMatrixLocation, 1, GL_FALSE, &_modelMatrix[0][0]); //


	// Bind the buffer and switch it to an active buffer
	glBindVertexArray(_vaoID[0]);

	int timeId = glGetUniformLocation(_program1, "time");
	glUniform1f(timeId, time);

	glDrawElements(GL_TRIANGLES, 6 * (VERTEX_COUNT - 1)*(VERTEX_COUNT - 1), GL_UNSIGNED_INT, 0);

	// Unbind our Vertex Array Object
	glBindVertexArray(0);

	// Unbind the shader program
	glUseProgram(0);


}
