//
//  EnvMappingTexture.cpp
//  Texture for cube map
//
//
//

#include "EnvMappingTexture.h"
#include "External Libraries\SOIL2\include\SOIL2.h"
//#define GL_CLAMP_TO_EDGE 33071 

#ifdef WIN32
string  GLCubmapTex::_glsl_name[1] = { "skyBox" }; 
#endif

GLCubmapTex::GLCubmapTex()
{
	_textureIdx = -1;
	_texture_blend_mode = 0;
	_dirty = false;
}

GLCubmapTex::~GLCubmapTex()
{


}

/*!
Loads a texture from a file and creates the necessary texture object
@param path_and_file to the texture objects
@return int - the texture id when the texture was sucessfully loaded.
*/
int GLCubmapTex::loadAndCreateCubMap(const char* path_and_file1, const char* path_and_file2, const char* path_and_file3, const char* path_and_file4, const char* path_and_file5, const char* path_and_file6)
{
	//load 6 images into a new OpenGL cube map, forcing RGB 
	_texture_cube = SOIL_load_OGL_cubemap
	(
		path_and_file1, path_and_file2, path_and_file3, path_and_file4, path_and_file5, path_and_file6,
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
	);

	if (_texture_cube == 0) // Check for invalid input 
	{
		cout << "Could not open or find the cube map \n" << std::endl; 
		cout << "SOIL loading error: " << SOIL_last_result() << endl;
	}

	// Set a texture as active texture.
	glBindTexture(GL_TEXTURE_CUBE_MAP, _texture_cube);//_texture

		// seamless cubemap
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // makes the problemetic texture to go black

									 // Return the texture.
	return _texture_cube;

}




/*!
Adds the variables of this object to the shader program
*/
//virtual
bool GLCubmapTex::addVariablesToProgram(GLuint program, int variable_index)
{

	if (program == -1)return false; // no program exits


	GLint params;
	glGetProgramiv(program, GL_LINK_STATUS, &params);
	if (params == GL_FALSE)
	{
		cerr << "[GLCubmapTex] Program " << program << " has not been linked. Textures cannot be added." << endl;
		return false;
	}

	// enable the program
	glUseProgram(program);


	// get the location of a uniform variable. Note, the program must be linked at this position.
	// location of the texture in the glsl program
	_textureIdx = glGetUniformLocation(program, _glsl_name[0].c_str());
	checkUniform(_textureIdx, _glsl_name[0]);


	/*
	glActiveTexture tells OpenGL which texture unit we want to use. GL_TEXTURE0 is the first texture unit, so we will just use that.
	*/
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _texture_cube);
	glUniform1i(_textureIdx, 9);


	// update the variable
	dirty(program);

	return true;
}


/*!
The function indicates that the variables of this object require an update
*/
//virtual 
bool GLCubmapTex::dirty(GLuint program)
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


/*!
This sets the texture blend model
@param mode - the values 0,1, or 2
*/
bool GLCubmapTex::setTextureBlendMode(int mode)
{
	if (mode < 0 || mode > 2)return false;
	if (_texture_blend_mode == mode)return false;

	// write the texture blend mode
	_texture_blend_mode = mode;

	_dirty = true;

	return true;
}




