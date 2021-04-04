#pragma once

// stl include
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//#include "camera.h"

#include "HCI557Common.h"
#include "GLObject.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#define SIZE 800 
#define VERTEX_COUNT 128
#define MAX_HEIGHT 100 
#define MAX_PIXEL_COLOR 256


class Terrain : public GLObject
{
	// Ray intersection test has access to protected functions
	friend class RayIntersectionTest;
public:
	Terrain(int gridX = 1, int gridY = 1); //, Loader loader, ModelTexture texture
	~Terrain();

	/*!
	Draw the terrain
	*/
	void draw(void);
	//void generateTerrain(void); // return type :RawModel, parameter: Loader loader

	/*!
	Init the geometry object
	*/
	void init(void);


	/*!
	Get the height of terrain at particular coordinats to implement collision detection
	*/
	float getHeightofTerrain(float, float);
	float barryCentric_Inerpolation(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos);

private:


	/*!
	Create the vertex buffer object for this element
	*/
	virtual void initVBO(); //const char*

	/*
	Inits the shader program for this object
	*/
	virtual void initShader(void);

	float getHeight(int, int, Mat);

	glm::vec3 calculateNormal(int, int, Mat);

	//// the program
	GLuint                  _program1;


	//int                     _viewMatrixLocation;
	//int                     _projectionMatrixLocation;

	//glm::mat4               _modelMatrix; // Store the model matrix


									   // length of the line
	int                   _x;
	int                   _z;

protected:
	// The data
	vector<glm::vec3>       _vertices;

	unsigned int            _vaoID[1]; // Our Vertex Array Object

	unsigned int            _vboID[4]; // Our Vertex Buffer Object

};

