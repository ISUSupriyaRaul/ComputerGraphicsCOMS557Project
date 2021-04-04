//
//  TerrainRivTexture.cpp
//
//
//

#include "TerrainRivTexture.h"
#include "External Libraries\SOIL2\include\SOIL2.h"


#ifdef WIN32
string  GLMultiTexture1::_glsl_names[4] = { "CubeMap", "RiverMap", "noiseMap","GroundTexture" };
#endif

// GLMultiTexture1

GLMultiTexture1::GLMultiTexture1()
{

	_texture_5 = 0;
	_texture_6 = 0;
	_texture_7 = 0;
	_texture_8 = 0;

	_texture_blend_mode = 0;
	_textureIdx1 = -1;
	_textureIdx2 = -1;
	_textureIdx3 = -1;
	_textureIdx4 = -1;
	_textureBlendModelIdx = -1;

	_dirty = false;

}


GLMultiTexture1::~GLMultiTexture1()
{

}

//void GLMultiTexture1:: PrintError()
//{
//	GLenum err;
//	for (;;) {
//		err = glGetError();
//		if (err == GL_NO_ERROR) break;
//		cerr << "Error: \n"<< glewGetErrorString(err) << endl;
//	}
//}


/*!
Load two bitmap images as textures from files.
@param path_and_file_texture_5 - path and file of the first image.
@param path_and_file_texture_6 - path and file of the second image.
@param path_and_file_texture_7 - path and file of the third image.
@param path_and_file_texture_8 - path and file of the fourth image.
@return int - the texture id when the texture was sucessfully loaded.
*/

int GLMultiTexture1::loadAndCreateTextures1(const char* cubemap_path_and_file[], string path_and_file_texture_2, string path_and_file_texture_3, string path_and_file_texture_4)
{
	//GLenum err1;
	//while ((err1 = glGetError()) != GL_NO_ERROR) {
	//	cerr << "OpenGL error inside TerrainRivTexture.cpp: " << err1 << " Error string= " << gluErrorString(err1) << endl;
	//}

	unsigned int channels2;
	unsigned int width2;
	unsigned int height2;

	unsigned int channels3;
	unsigned int width3;
	unsigned int height3;

	unsigned int channels4;
	unsigned int width4;
	unsigned int height4;

	unsigned char* data2 = loadBitmapFile(path_and_file_texture_2, channels2, width2, height2);
	unsigned char* data3 = loadBitmapFile(path_and_file_texture_3, channels3, width3, height3);
	unsigned char* data4 = loadBitmapFile(path_and_file_texture_4, channels4, width4, height4);

	if (data2 == NULL || data3 == NULL || data4 == NULL)return -1;

	//**********************************************************************************************
	// Texture generation - cubemap

	_texture_5 = SOIL_load_OGL_cubemap
	(
		cubemap_path_and_file[0], cubemap_path_and_file[1], cubemap_path_and_file[2], cubemap_path_and_file[3], cubemap_path_and_file[4], cubemap_path_and_file[5],
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
	);

	if (_texture_5 == 0) // Check for invalid input 
	{
		cout << "Could not open or find the cube map \n" << std::endl;
		cout << "SOIL loading error: " << SOIL_last_result() << endl;
	}

	// Set a texture as active texture.
	glBindTexture(GL_TEXTURE_CUBE_MAP, _texture_5);

	// Change the parameters of your texture units.
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	//**********************************************************************************************
	// Texture river map

	// Activate a texture unit
	glActiveTexture(GL_TEXTURE6);
	glGenTextures(1, &_texture_6);
	glBindTexture(GL_TEXTURE_2D, _texture_6);

	// Change the parameters of your texture units.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Create a texture and load it to your graphics hardware. This texture is automatically associated
	// with texture 0 and the textuer variable "texture" / the active texture.
	if (channels2 == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_BGR, GL_UNSIGNED_BYTE, data2);
	else if (channels2 == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_BGRA, GL_UNSIGNED_BYTE, data2);


	// Delete your loaded data
	free(data2);

	//**********************************************************************************************
	// Texture mud

	// Activate a texture unit
	glActiveTexture(GL_TEXTURE7);
	glGenTextures(1, &_texture_7);
	glBindTexture(GL_TEXTURE_2D, _texture_7);

	// Change the parameters of your texture units.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Create a texture and load it to your graphics hardware. This texture is automatically associated
	// with texture 0 and the textuer variable "texture" / the active texture.
	if (channels3 == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width3, height3, 0, GL_BGR, GL_UNSIGNED_BYTE, data3);
	else if (channels3 == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width3, height3, 0, GL_BGRA, GL_UNSIGNED_BYTE, data3);

	//**********************************************************************************************

	// Delete your loaded data
	free(data3);

	//**********************************************************************************************
	// Texture mud

	// Activate a texture unit
	glActiveTexture(GL_TEXTURE8);
	glGenTextures(1, &_texture_8);
	glBindTexture(GL_TEXTURE_2D, _texture_8);

	// Change the parameters of your texture units.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	// Create a texture and load it to your graphics hardware. This texture is automatically associated
	// with texture 0 and the textuer variable "texture" / the active texture.
	if (channels4 == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width4, height4, 0, GL_BGR, GL_UNSIGNED_BYTE, data4);
	else if (channels4 == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width4, height4, 0, GL_BGRA, GL_UNSIGNED_BYTE, data4);


	// Delete your loaded data
	free(data4);
	//glBindTexture(GL_TEXTURE_2D, 0);

	// Return the texture.
	return _texture_5;



}

/*!
This sets the texture blend model
@param mode - the values 0,1, or 2
@return true, when a new mode was set, false when current and new mode are equal
*/
bool GLMultiTexture1::setTextureBlendMode(int mode)
{
	if (mode < 0 || mode > 2)return false;
	if (_texture_blend_mode == mode)return false;

	// write the texture blend mode
	_texture_blend_mode = mode;

	_dirty = true;

	return true;

}


/*!
Adds the variables of this object to the shader program
*/
//virtual
bool GLMultiTexture1::addVariablesToProgram(GLuint program, int variable_index)
{
	if (program == -1)return false; // no program exits


	GLint params;
	glGetProgramiv(program, GL_LINK_STATUS, &params);
	if (params == GL_FALSE)
	{
		cerr << "[GLTexture] Program " << program << " has not been linked. Textures cannot be added." << endl;
		return false;
	}

	// enable the program
	glUseProgram(program);


	// get the location of a uniform variable. Note, the program must be linked at this position.
	// location of the texture in the glsl program
	_textureIdx1 = glGetUniformLocation(program, _glsl_names[0].c_str());
	checkUniform(_textureIdx1, _glsl_names[0]);

	_textureIdx2 = glGetUniformLocation(program, _glsl_names[1].c_str());
	checkUniform(_textureIdx2, _glsl_names[1]);

	_textureIdx3 = glGetUniformLocation(program, _glsl_names[2].c_str());
	checkUniform(_textureIdx3, _glsl_names[2]);

	_textureIdx4 = glGetUniformLocation(program, _glsl_names[3].c_str());
	checkUniform(_textureIdx4, _glsl_names[3]);

	//****************************************************************************************************
	// Link the texture to the uniform variable and texture unit 0;

	/*
	glActiveTexture tells OpenGL which texture unit we want to use. GL_TEXTURE0 is the first texture unit, so we will just use that.
	*/
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _texture_5); 	// CUBE MAP
	glUniform1i(_textureIdx1, 5);


	/*
	glActiveTexture tells OpenGL which texture unit we want to use. GL_TEXTURE0 is the first texture unit, so we will just use that.
	*/
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, _texture_6);
	glUniform1i(_textureIdx2, 6);


	/*
	glActiveTexture tells OpenGL which texture unit we want to use. GL_TEXTURE0 is the first texture unit, so we will just use that.
	*/
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, _texture_7);
	glUniform1i(_textureIdx3, 7);

	/*
	glActiveTexture tells OpenGL which texture unit we want to use. GL_TEXTURE0 is the first texture unit, so we will just use that.
	*/
	glActiveTexture(GL_TEXTURE8); //GL_TEXTURE3
	glBindTexture(GL_TEXTURE_2D, _texture_8);
	glUniform1i(_textureIdx4, 8); //3


	// update the variable
	dirty(program);

	return true;

}


/*!
The function indicates that the variables of this object require an update
*/
//virtual
bool GLMultiTexture1::dirty(GLuint program)
{
	// enable the program
	glUseProgram(program);

	// write the texture blend mode
	glUniform1i(_textureBlendModelIdx, _texture_blend_mode);


	// disable the program
	glUseProgram(0);

	_dirty = false;

	return true;

}


