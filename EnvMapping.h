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
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

#define CMAPSIZE 800 


class CubeMap : public GLObject
{
	// Ray intersection test has access to protected functions
	friend class RayIntersectionTest;
public:
	CubeMap(); //, Loader loader, ModelTexture texture
	~CubeMap();

	/*!
	Draw the CubeMap
	*/
	void draw(void);

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


protected:
	// The data
	static const GLfloat _Vertices[];

	unsigned int            _vaoID[1]; // Our Vertex Array Object

	unsigned int            _vboID[1]; // Our Vertex Buffer Object

};

