#include "Grass.h"
#include "ImgLoader.h"

bool CheckShader(GLuint shader, GLenum shader_type)
{
	GLint compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		GLint info_len = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);

		if (info_len)
		{
			char* buf = (char*)malloc(info_len);
			if (buf) {
				glGetShaderInfoLog(shader, info_len, NULL, buf);
				cout << "Could not compile shader shader_type " << shader_type << ":\n" << buf << endl;
				free(buf);
			}
			glDeleteShader(shader);
			shader = 0;
		}
		return false;
	}
	return true;

}


glm::mat4 projectionMatrix3; // Store the projection matrix
glm::mat4 viewMatrix3; // Store the view matrix

GLuint _texture1;

static const string vs_string_CoordSystem_410_1 = "#version 410 core                                                 \n"
"                                                                   \n"
"uniform mat4 projectionMatrix;                                    \n"
"uniform mat4 viewMatrix;                                           \n"
"uniform mat4 modelMatrix;                                          \n"
"in vec3 in_Position;                                               \n"
"in vec3 in_Normal;                                                  \n"
"in vec2 in_TexCoord;                                               \n"
"in vec3 in_Color;                                                  \n"
"out vec4 pass_Color;                                               \n"
"out vec2 pass_TexCoord;                                            \n"
"                                                                   \n"
"void main(void)                                                   \n"
"{                                                                 \n"
"    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);  \n"
"    pass_Color = vec4(in_Color, 1.0);                              \n"
"    pass_TexCoord = in_TexCoord * 40.0;                                  \n"
"}                                                                 \n";

// Fragment shader source code. This determines the colors in the fragment generated in the shader pipeline. In this case, it colors the inside of our triangle specified by our vertex shader.
static const string fs_string_CoordSystem_410_1 = 
"#version 410 core                                                 \n"
"uniform sampler2D GrassTex;									   \n"
"in vec4 pass_Color;                                                 \n"
"in vec2 pass_TexCoord;                                                 \n"
"out vec4 color;                                                    \n"
"void main(void)                                                   \n"
"{                                                                 \n"
"  vec4 tex_color =  texture(GrassTex, pass_TexCoord.xy);         \n"
"  color =  0.1 * pass_Color + 0.8 * tex_color;                     \n"
" // color = pass_Color;                               \n"
"}                                                                 \n";




Terrain::Terrain(int gridX, int gridY) :
_x(gridX*SIZE), _z(gridY*SIZE) //_model(generateTerrain(loader))
{
	_program1 = -1;
	initShader();
	initVBO();
}


Terrain::~Terrain()
{
	// Program clean up when the window gets closed.
	glDeleteVertexArrays(1, _vaoID);
	glDeleteProgram(_program1);
}

/*!
Draw the objects
*/
void Terrain::generateTerrain(void)
{

	// Enable the shader program
	glUseProgram(_program1);

	// this changes the camera location
	glm::mat4 rotated_view = viewMatrix3 * GetRotationMatrix();
	glUniformMatrix4fv(_viewMatrixLocation, 1, GL_FALSE, &rotated_view[0][0]); // send the view matrix to our shader

	// Bind the buffer and switch it to an active buffer
	glBindVertexArray(_vaoID[0]);

	int idg = glGetUniformLocation(_program1, "GrassTex");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture1);
	glUniform1i(idg, 0);

	//glLineWidth((GLfloat)3.0);
	// Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0,4);


//	glDrawElements(GL_TRIANGLES, 6 * (VERTEX_COUNT - 1)*(VERTEX_COUNT - 1), GL_UNSIGNED_INT, 0); 

	// Disable vertex attrib array
	//glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
	//glDisableVertexAttribArray(2);
	//glDisableVertexAttribArray(3);

	// Unbind our Vertex Array Object
	glBindVertexArray(0);
	
	// Unbind our texture unit
	glBindTexture(GL_TEXTURE_2D, 0); 

	// Unbind the shader program
	glUseProgram(0);


}

/*!
Create the vertex buffer object for this element
*/
void Terrain::initVBO(void)
{
	float* verticesTexG = new float[30];  // Vertices and texture coordinates for our Grass
	float *colorsG = new float[18]; // Colors for our vertices
	float *normalsG = new float[18]; // normals for our vertices

									// X				Y					Z					U					V						
	verticesTexG[0] = -0.5; verticesTexG[1] = -0.5; verticesTexG[2] = 0.5; verticesTexG[3] = 0.0; verticesTexG[4] = 0.0; // Bottom left corner
	colorsG[0] = 1.0; colorsG[1] = 1.0; colorsG[2] = 1.0; // Bottom left corner
	normalsG[0] = -0.5; normalsG[1] = -0.5; normalsG[2] = 0.5; // Bottom left corner


	verticesTexG[5] = -0.5; verticesTexG[6] = 0.5; verticesTexG[7] = 0.5; verticesTexG[8] = 0.0;  verticesTexG[9] = 1.0;  // Top left corner
	colorsG[3] = 1.0; colorsG[4] = 0.0; colorsG[5] = 0.0; // Top left corner
	normalsG[3] = -0.5; normalsG[4] = 0.5; normalsG[5] = 0.5; // Top left corner


	verticesTexG[10] = 0.5; verticesTexG[11] = 0.5; verticesTexG[12] = 0.5; verticesTexG[13] = 1.0; verticesTexG[14] = 1.0;   // Top Right corner
	colorsG[6] = 0.0; colorsG[7] = 1.0; colorsG[8] = 0.0; // Top Right corner
	normalsG[6] = 0.5; normalsG[7] = 0.5; normalsG[8] = 0.5; // Top Right corner


	verticesTexG[15] = 0.5; verticesTexG[16] = -0.5; verticesTexG[17] = 0.5; verticesTexG[18] = 1.0; verticesTexG[19] = 0.0;  // Bottom right corner
	colorsG[9] = 0.0; colorsG[10] = 0.0; colorsG[11] = 1.0; // Bottom right corner
	normalsG[9] = 0.5; normalsG[10] = -0.5; normalsG[11] = 0.5; // Bottom right corner


	verticesTexG[20] = -0.5; verticesTexG[21] = -0.5; verticesTexG[22] = 0.5; verticesTexG[23] = 0.0;  verticesTexG[24] = 0.0; // Bottom left corner
	colorsG[12] = 1.0; colorsG[13] = 1.0; colorsG[14] = 1.0; // Bottom left corner
	normalsG[12] = -0.5; normalsG[13] = -0.5; normalsG[14] = 0.5; // Bottom left corner


	verticesTexG[25] = 0.5; verticesTexG[26] = 0.5; verticesTexG[27] = 0.5; verticesTexG[28] = 1.0; verticesTexG[29] = 1.0; // Top Right corner
	colorsG[15] = 0.0; colorsG[16] = 1.0; colorsG[17] = 0.0;
	normalsG[15] = 0.5; normalsG[16] = 0.5; normalsG[17] = 0.5; // Top Right corner
	
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
			vertices[vertexPointer * 3 + 1] = 0;
			vertices[vertexPointer * 3 + 2] = (float)i / ((float)VERTEX_COUNT - 1) * SIZE;
			normals[vertexPointer * 3] = 0;
			normals[vertexPointer * 3 + 1] = 1;
			normals[vertexPointer * 3 + 2] = 0;
			colors[vertexPointer * 3] = 1.0;
			colors[vertexPointer * 3 + 1] = 0.5;
			colors[vertexPointer * 3 + 2] = 0.0;
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

	int logTexCoord = glGetAttribLocation(_program1, "in_TexCoord");
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


	// Load the color map and noise map 
	ImgLoader* texture = new ImgLoader();
	unsigned int width = 128;
	unsigned int height = 128;
	unsigned int channels = 3;
	unsigned char* data = texture->Load("grassTexture_preview.bmp", channels, width, height); //grass-texture_bmp.bmp

	// For terrain textuer
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &_texture1);
	glBindTexture(GL_TEXTURE_2D, _texture1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	free(data);

	cout << "For terrain grid (" << _x << "," << _z  <<") : texture width " << width << " texture height " << height << " texture channels " << channels << "\n" << endl;

	glUseProgram(0);

}


/*
Inits the shader program for this object
*/
void Terrain::initShader(void)
{

	// Vertex shader source code. This draws the vertices in our window. We have 3 vertices since we're drawing an triangle.
	// Each vertex is represented by a vector of size 4 (x, y, z, w) coordinates.
	// static const string vertex_code = vs_string_CoordSystem;
	static const char * vs_source = vs_string_CoordSystem_410_1.c_str();

	// Fragment shader source code. This determines the colors in the fragment generated in the shader pipeline. In this case, it colors the inside of our triangle specified by our vertex shader.
	// static const string fragment_code = fs_string_CoordSystem;
	static const char * fs_source = fs_string_CoordSystem_410_1.c_str();

	// This next section we'll generate the OpenGL program and attach the shaders to it so that we can render our triangle.
	_program1 = glCreateProgram();


	/*glBindAttribLocation(_program1, 0, "in_Position");
	glBindAttribLocation(_program1, 1, "in_Normal");
	glBindAttribLocation(_program1, 2, "in_TexCoord");
	glBindAttribLocation(_program1, 3, "in_Color");*/

	// We create a shader with our fragment shader source code and compile it.
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);
	bool ret =  CheckShader(fs,  GL_FRAGMENT_SHADER);

	// We create a shader with our vertex shader source code and compile it.
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);
	 ret = CheckShader(vs, GL_VERTEX_SHADER);

	// We'll attach our two compiled shaders to the OpenGL program.
	glAttachShader(_program1, vs);
	glAttachShader(_program1, fs);


	glLinkProgram(_program1);

	glUseProgram(_program1);

	_modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(_x, 0.0f,_z));

	projectionMatrix3 = glm::perspective(1.1f, (float)1900 / (float)1000, 0.1f, 1000.f);
	viewMatrix3 = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));


	_projectionMatrixLocation = glGetUniformLocation(_program1, "projectionMatrix"); // Get the location of our projection matrix in the shader
	_viewMatrixLocation = glGetUniformLocation(_program1, "viewMatrix"); // Get the location of our view matrix in the shader
	int modelMatrixLocation = glGetUniformLocation(_program1, "modelMatrix"); // Get the location of our model matrix in the shader


	glUniformMatrix4fv(_projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix3[0][0]); // Send our projection matrix to the shader
	glUniformMatrix4fv(_viewMatrixLocation, 1, GL_FALSE, &viewMatrix3[0][0]); // Send our view matrix to the shader
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &_modelMatrix[0][0]); // Send our model matrix to the shader


	


	glUseProgram(0);


}