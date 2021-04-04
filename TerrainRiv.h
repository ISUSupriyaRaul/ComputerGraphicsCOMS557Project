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

#define SIZE 300 
#define VERTEX_COUNT 128
#define MAX_HEIGHT 100 
#define MAX_PIXEL_COLOR 256
#define MAP_SIZE 128


class TerrainRiv : public GLObject
{
	// Ray intersection test has access to protected functions
	friend class RayIntersectionTest;
public:
	TerrainRiv(int gridX = 1, int gridY = 1); //, Loader loader, ModelTexture texture
	~TerrainRiv();

	/*!
	Draw the TerrainRiv
	*/
	void draw(void);
	//void generateTerrain(void); // return type :RawModel, parameter: Loader loader

	/*!
	Init the geometry object
	*/
	void init(void);


private:


	/*!
	Create the vertex buffer object for this element
	*/
	virtual void initVBO(); //const char*

							/*
							Inits the shader program for this object
							*/
	virtual void initShader(void);

	//// the program
	GLuint                  _program1;


	// length of the line
	int                   _x;
	int                   _z;

protected:
	// The data
	vector<glm::vec3>       _vertices;

	unsigned int            _vaoID[1]; // Our Vertex Array Object

	unsigned int            _vboID[4]; // Our Vertex Buffer Object

};

