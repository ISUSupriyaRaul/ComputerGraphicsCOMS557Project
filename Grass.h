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


#include "HCI557Common.h"

using namespace std;

#define SIZE 20 
#define VERTEX_COUNT 10 

class Terrain
{
public:
	Terrain(int gridX = 1, int gridY= 1); //, Loader loader, ModelTexture texture
	~Terrain();

	/*!
	Draw the terrain
	*/
	//void draw(void);
	void generateTerrain(void); // return type :RawModel, parameter: Loader loader



private:


	/*!
	Create the vertex buffer object for this element
	*/
	void initVBO(void);
	void initVBOGrass(void);

	/*
	Inits the shader program for this object
	*/
	void initShader(void);
	void initShaderGrass(void);




	// the program
	GLuint                  _program1;
	int                     _viewMatrixLocation;
	int                     _projectionMatrixLocation;

	glm::mat4               _modelMatrix; // Store the model matrix


	unsigned int            _vaoID[2]; // Our Vertex Array Object

	unsigned int            _vboID[8]; // Our Vertex Buffer Object

									   // length of the line
	int                   _x;
	int                   _z;
	//RawModel				_model;
	//ModelTexture			_texture;

};

