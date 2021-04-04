#include "Terrain.h"
#include "ImgLoader.h"
#include "Shaders.h"
#include <algorithm>
#include <opencv2/opencv.hpp>

#include <cassert>
using namespace cv;
using namespace std;

glm::mat4 projectionMatrix3; // Store the projection matrix
glm::mat4 viewMatrix3; // Store the view matrix


GLuint _texture1, _texture2, _texture3, _texture4;
float heights[VERTEX_COUNT][VERTEX_COUNT] = { 0 };


Terrain::Terrain(int gridX, int gridY) :
	_x(gridX*SIZE), _z(gridY*SIZE)
{

}


Terrain::~Terrain()
{
	// Program clean up when the window gets closed.
	glDeleteVertexArrays(1, _vaoID);
	glDeleteProgram(_program1);
}

/*!
Init the geometry object
*/
void Terrain::init(void)
{
		initShader();
		initVBO();
}

// BarryCentric_Inerpolation to calculate the height value interpolating the heights at the vertices of the triangle in which the tree is present
float Terrain::barryCentric_Inerpolation(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos) 
{
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

float Terrain::getHeightofTerrain(float worldX, float worldZ)
{
	float terrainX = worldX - this->_x;
	float terrainZ = worldZ - this->_z;
	float gridSquareSize = SIZE / (float)(VERTEX_COUNT - 1); // finds the number of grids in one side of the terrain 
	int gridXt = (int) floor(terrainX / gridSquareSize); // find the tree is in which grid square 
	int gridZt = (int) floor(terrainZ / gridSquareSize); // find the tree is in which grid square 
	if (gridXt >= (VERTEX_COUNT - 1) || gridZt >= (VERTEX_COUNT - 1) || gridXt < 0 || gridZt < 0) { // test if this deduced position is on the terrain
		return 0;
	}
	float xCoord =  fmod(terrainX, gridSquareSize) / gridSquareSize; //   find the coordinate between o and 1 where the tree is in the grid exactly
	float zCoord = fmod(terrainZ, gridSquareSize) / gridSquareSize; //   find the coordinate between o and 1 where the tree is in the grid exactly
	float answer;
	if (xCoord <= (1- zCoord)) {
		answer = barryCentric_Inerpolation(glm::vec3(0, heights[gridXt][gridZt], 0), glm::vec3(1,heights[gridXt + 1][gridZt], 0), glm::vec3(0,heights[gridXt][gridZt + 1], 1), glm::vec2(xCoord, zCoord));
	}
	else {
		answer = barryCentric_Inerpolation(glm::vec3(1, heights[gridXt+1][gridZt], 0), glm::vec3(1, heights[gridXt + 1][gridZt + 1], 1), glm::vec3(0, heights[gridXt][gridZt + 1], 1), glm::vec2(xCoord, zCoord));
	}
	return answer;
}


glm::vec3 Terrain::calculateNormal(int x, int z, Mat img)
{
	float heightL = 0, heightR=0, heightD =0, heightU=0;
	if (x - 1 >= 0 && x+1 <= 127 && z- 1 >= 0 && z + 1 <= 127) {
		try {
			heightL = getHeight(x - 1, z, img);
			heightR = getHeight(x + 1, z, img);
			heightD = getHeight(x, z - 1, img);
			heightU = getHeight(x, z + 1, img);
		}
		catch (cv::Exception & e)
		{
			cerr << e.msg << endl; // output exception message
		}
	}
		
	glm::vec3 normal = glm::vec3(heightL - heightR, 2.0, heightD - heightU);
	normal = normalize(normal);
	return normal;
}

float Terrain::getHeight(int x, int z, Mat img)
{
	
	float value = img.at<float>(z, x);

	float height = value;
	height /= MAX_PIXEL_COLOR/2; // converted to a float betwen 0 and 1
	height = height * 2 - 1;
	height *= MAX_HEIGHT;
	return height;
	
}

/*!
Create the vertex buffer object for this element
*/
void Terrain::initVBO() // const char* HeightMap
{
	Mat img2 = imread("Textures_&_Objects/heightmap128.bmp", IMREAD_COLOR); 
	Mat img = img2.clone();
	cv::cvtColor(img2, img, CV_RGBA2GRAY); // change the number of channels
	//imwrite("grey.png", img);
	Mat img_float = cv::Mat(img.cols, img.rows, CV_32FC1); //CV_32FC1
	img.convertTo(img_float, CV_32FC1); 

	if (!img2.data) // Check for invalid input 
	{
		cout << "Could not open or find the image" << std::endl;
	}
	

	int count = VERTEX_COUNT * VERTEX_COUNT;
	int countI = (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1);
	float* vertices = new float[count * 3]; 
	float* colors = new float[count * 3];
	float* normals = new float[count * 3]; 
	float* textureCoords = new float[count * 2]; 
	int* indices = new int[6 * (VERTEX_COUNT - 1)*(VERTEX_COUNT - 1)]; 
	int vertexPointer = 0;
	for (int i = 0; i<VERTEX_COUNT; i++) {
		for (int j = 0; j<VERTEX_COUNT; j++) {
			vertices[vertexPointer * 3] = (float)j / ((float)VERTEX_COUNT - 1) * SIZE;
			float height = getHeight(j, i, img_float);
			if ((j == 0) || (i == 0) || (j == VERTEX_COUNT - 1) || (i == VERTEX_COUNT - 1)) height = -30;
			vertices[vertexPointer * 3 + 1] = height;
			heights[j][i] = height;
			vertices[vertexPointer * 3 + 2] = (float)i / ((float)VERTEX_COUNT - 1) * SIZE;
			glm::vec3 normal = calculateNormal(j, i, img_float);
			normals[vertexPointer * 3] = normal.x;//0;
			normals[vertexPointer * 3 + 1] = normal.y; //1;
			normals[vertexPointer * 3 + 2] = normal.z; //0;
			colors[vertexPointer * 3] = 1.0;
			colors[vertexPointer * 3 + 1] = 0.5;
			colors[vertexPointer * 3 + 2] = 0.0;
			textureCoords[vertexPointer * 2] = (float)j / ((float)VERTEX_COUNT - 1); 
			textureCoords[vertexPointer * 2 + 1] = (float)i / ((float)VERTEX_COUNT - 1);
			//assert(textureCoords[vertexPointer * 2 + 1);
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
void Terrain::initShader(void)
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
void Terrain::draw(void) // generateTerrain
{

	// Enable the shader program
	glUseProgram(_program1);

	// this changes the camera location
	glm::mat4 rotated_view = rotatedViewMatrix();
	glUniformMatrix4fv(_viewMatrixLocation, 1, GL_FALSE, &rotated_view[0][0]); // send the view matrix to our shader
	glUniformMatrix4fv(_inverseViewMatrixLocation, 1, GL_FALSE, &invRotatedViewMatrix()[0][0]);
	glUniformMatrix4fv(_modelMatrixLocation, 1, GL_FALSE, &_modelMatrix[0][0]); //

	// Bind the buffer and switch it to an active buffer
	glBindVertexArray(_vaoID[0]);


	glDrawElements(GL_TRIANGLES, 6 * (VERTEX_COUNT - 1)*(VERTEX_COUNT - 1), GL_UNSIGNED_INT, 0);

	// Unbind our Vertex Array Object
	glBindVertexArray(0);

	// Unbind the shader program
	glUseProgram(0);


}
