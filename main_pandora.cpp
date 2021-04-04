//
//  main.cpp
//

// stl include
#include <iostream>
#include <string>
#include <ctime>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>


// glfw includes
#include <GLFW/glfw3.h>

// include local files
#include "controls.h"
#include "HCI557Common.h"
#include "CoordSystem.h"
#include "Terrain.h"
#include "TerrainRiv.h"
#include "camera.h"
#include "EnvMapping.h"
#include "Sphere3D.h"
#include "GLObjectObj.h"

// The handle to the window object
GLFWwindow*         window;
// Define some of the global variables we're using for this sample
//GLuint program;
/* A trackball to move and rotate the camera view */
extern Trackball trackball;
extern float heights;

//clock_t LastFrameTime;
GLfloat sunX = 20.0, sunY = 20.0, sunZ = 0.0; //20.0, 20.0

int main(int argc, const char * argv[])
{
	// Init the GLFW Window
	window = initWindow();
	// Setting window properties
	glfwSetWindowTitle(window, "Sue's Pandora");
	glfwSetWindowPos(window, 10, 40);
	glfwSetWindowSize(window, 1900, 1000);
	//Retrieve the current size of the framebuffer of a window, to set the viewport size
	int fbwidth, fbheight;
	glfwGetFramebufferSize(window, &fbwidth, &fbheight);
	glViewport(0, 0, fbwidth, fbheight);
	// Init the glew api
	initGlew();
	SetCameraManipulator(CameraTypes::CAMERA_MANIPULATOR);

	// Prepares some defaults
	CoordSystemRenderer* coordinate_system_renderer = new CoordSystemRenderer(10.0);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// light sources and material for terrain
	// create light source
	GLDirectLightSource  Directional_light_source;
	Directional_light_source._lightPos = glm::vec4(sunX, sunY, sunZ, 0.0);
	Directional_light_source._ambient_intensity = 1.2;
	Directional_light_source._specular_intensity = 3.0; //5.5
	Directional_light_source._diffuse_intensity = 1.0;
	Directional_light_source._attenuation_coeff = 0.0;


	// Create a material object
	GLMaterial material_0;
	material_0._diffuse_material = glm::vec3(0.8, 0.8, 0.8); 
	material_0._ambient_material = glm::vec3(0.6, 0.6, 0.6); 
	material_0._specular_material = glm::vec3(1.0, 1.0, 1.0);
	material_0._shininess = 12.0; 
	material_0._transparency = 1.0;


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create terrain object
	GLAppearance* apperance_terrain = new GLAppearance("shaders/terrain_vertex_shader.vs", "shaders/terrain_fragment_shader.fs");
	// add the light to this apperance object
	apperance_terrain->addLightSource(Directional_light_source);
	// Add the material to the apperance object
	apperance_terrain->setMaterial(material_0);
	// Add texture to the terrain
	GLMultiTexture* terraintexture = new GLMultiTexture();
	int texid0 = terraintexture->loadAndCreateTextures("Textures_&_Objects/TexturesGrass.bmp","Textures_&_Objects/blendMap_preview.bmp","Textures_&_Objects/dirt_texture.bmp", "Textures_&_Objects/Snow0.bmp"); //blendMap_river2 dirt_texture calm_blue_water  sand_walkway1
	apperance_terrain->setTexture(terraintexture);
	apperance_terrain->finalize();

	Terrain* Terrain_Renderer = new Terrain(0, -1);
	Terrain* Terrain_Renderer1 = new Terrain(0, 0);
	Terrain* Terrain_Renderer2 = new Terrain(-1, -1);
	Terrain* Terrain_Renderer3 = new Terrain(-1, 0);

	Terrain_Renderer->setApperance(*apperance_terrain);
	Terrain_Renderer->init();
	Terrain_Renderer1->setApperance(*apperance_terrain);
	Terrain_Renderer1->init();
	Terrain_Renderer2->setApperance(*apperance_terrain);
	Terrain_Renderer2->init();
	Terrain_Renderer3->setApperance(*apperance_terrain);
	Terrain_Renderer3->init();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create terrain with water object
	GLAppearance* apperance_terrain1 = new GLAppearance("shaders/terrain_vertex_shader1.vs", "shaders/terrain_fragment_shader1.fs");
	apperance_terrain1->addLightSource(Directional_light_source);
	apperance_terrain1->setMaterial(material_0);
	GLMultiTexture1* terraintexture1 = new GLMultiTexture1();
	const char* cubemap_path_and_file[] = { "Textures_&_Objects/Skybox_Images2/right.png",
		"Textures_&_Objects/Skybox_Images2/left.png",
		"Textures_&_Objects/Skybox_Images2/top.png",
		"Textures_&_Objects/Skybox_Images2/bottom.jpg",
		"Textures_&_Objects/Skybox_Images2/back.png",
		"Textures_&_Objects/Skybox_Images2/front.png" }; 

	// To implement environment mapping cub map is used
	int texid2 = terraintexture1->loadAndCreateTextures1(cubemap_path_and_file, "Textures_&_Objects/water_bmp.bmp", "Textures_&_Objects/waves1.bmp", "Textures_&_Objects/dirt_texture.bmp");

	apperance_terrain1->setTexture(terraintexture1);
	apperance_terrain1->finalize();
	TerrainRiv* TerrainRiv_Renderer1 = new TerrainRiv(0, 0);
	TerrainRiv_Renderer1->setApperance(*apperance_terrain1);
	TerrainRiv_Renderer1->init();
	TerrainRiv* TerrainRiv_Renderer2 = new TerrainRiv(0, -1);
	TerrainRiv_Renderer2->setApperance(*apperance_terrain1);
	TerrainRiv_Renderer2->init();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create tree object
	GLAppearance* apperance_tree = new GLAppearance("shaders/tree_vertex_shader.vs", "shaders/tree_fragment_shader.fs");

	// add the light to this apperance object
	apperance_tree->addLightSource(Directional_light_source);

	// Add the material to the apperance object
	apperance_tree->setMaterial(material_0);
	// Add a texture to the trees
	GLTexture* treetexture = new GLTexture();
	int texid1 = treetexture->loadAndCreateTexture("Textures_&_Objects/TreeTexture.bmp");
	apperance_tree->setTexture(treetexture);
	apperance_tree->finalize();

	// Create multiple trees 
	GLObjectObj* treeObj[50]; //5 good
	std::vector<GLObjectObj*> treeVector;
	glm::mat4 matrix;
	float randomPosx = 0.0;
	float randomPosz = 0.0;
	for (int i = 1; i <= 50; i++)
	{
		treeObj[i]= new GLObjectObj("Textures_&_Objects/myTree2.1.obj"); 
		treeObj[i]->setApperance(*apperance_tree);
		treeObj[i]->init();
		randomPosx = (float)(rand() % 801 + (-400)); // produces random digit among 801 distinct integers between -400 and +400 (including both)
		randomPosz = (float)(rand() % 801 + (-400)); // produces random digit among 801 distinct integers between -400 and +400 (including both)
		//randomPosx = (float)(rand() % 401 + (-200)); // produces random digit among 401 distinct integers between -200 and +200 (including both)
		//randomPosz = (float)(rand() % 401 + (-200)); // produces random digit among 401 distinct integers between -200 and +200 (including both)
		float treeterrainHeight=0.0;
		
		//find the height of the terrain at this random position to fix the tree at this height
		if (signbit(randomPosx) == 0 && signbit(randomPosz) == 1) {
			treeterrainHeight = Terrain_Renderer->getHeightofTerrain(randomPosx, randomPosz);
			matrix = glm::translate(glm::mat4(), glm::vec3(randomPosx, treeterrainHeight, randomPosz));
		}
		else if (signbit(randomPosx) == 1 && signbit(randomPosz) == 1) {
			treeterrainHeight = Terrain_Renderer2->getHeightofTerrain(randomPosx, randomPosz);
			matrix = glm::translate(glm::mat4(), glm::vec3(randomPosx, treeterrainHeight, randomPosz));
		}
		else if (signbit(randomPosx) == 1 && signbit(randomPosz) == 0) {
			treeterrainHeight = Terrain_Renderer3->getHeightofTerrain(randomPosx, randomPosz);
			matrix = glm::translate(glm::mat4(), glm::vec3(randomPosx, treeterrainHeight, randomPosz));
		}
		treeObj[i]->setMatrix(matrix);
		treeVector.push_back(treeObj[i]);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create Cub Map object
	GLAppearance* apperance_skybox = new GLAppearance("shaders/skybox_vertex_shader.vs", "shaders/skybox_fragment_shader.fs");
	// Add a texture to the enviroment
	GLCubmapTex* cubmaptexture = new GLCubmapTex();
	int CubMapid = cubmaptexture->loadAndCreateCubMap("Textures_&_Objects/Skybox_Images2/right.png",
		"Textures_&_Objects/Skybox_Images2/left.png",
		"Textures_&_Objects/Skybox_Images2/top.png",
		"Textures_&_Objects/Skybox_Images2/bottom.jpg",
		"Textures_&_Objects/Skybox_Images2/back.png",
		"Textures_&_Objects/Skybox_Images2/front.png");

	apperance_skybox->setTexture(cubmaptexture);
	apperance_skybox->finalize();

	CubeMap* Skybox_Renderer = new CubeMap();
	Skybox_Renderer->setApperance(*apperance_skybox);
	Skybox_Renderer->init();

	// Set up our background color
	static const GLfloat clear_color[] = { 0.6f, 0.7f, 1.0f, 1.0f };
	static const GLfloat clear_depth[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// This sets the camera to a new location
	// the first parameter is the eye position, the second the center location, and the third the up vector. 
	/*!
	Global function to set the view matrix as look at
	*/
	SetViewAsLookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	/*!
	Set the view frustum.
	*/
	SetPerspectiveViewFrustum(1.1f, (float)1900 / (float)1000, 0.1f, 1000.f);

	// Enable depth test
	//it allows us to keep the distance value after we proejct each object to a 2d canvas.
	glEnable(GL_DEPTH_TEST);
	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// This is our render loop. As long as our window remains open (ESC is not pressed), we'll continue to render things.
	while (!glfwWindowShouldClose(window))
	{
		// Clear the entire buffer with our green color (sets the background to be green).
		glClearBufferfv(GL_COLOR, 0, clear_color);
		glClearBufferfv(GL_DEPTH, 0, clear_depth);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//// This renders the objects

		// Set the trackball locatiom
		SetTrackballLocation(GetCurrentCameraMatrix(), GetCurrentCameraTranslation(), GetCurrentTarget());
		// this draws the coordinate system
		//coordinate_system_renderer->draw();

		// draw the terrain
		Terrain_Renderer->draw();
		Terrain_Renderer1->draw(); 
		TerrainRiv_Renderer1->draw(); // terrain with pond
		TerrainRiv_Renderer2->draw(); // terrain with pond
		Terrain_Renderer2->draw(); 
		Terrain_Renderer3->draw(); 

		// draw trees
		for (int i = 1; i <= treeVector.size(); i++)
		{	
			treeObj[i]->draw();
		}

		// draw the skybox
		Skybox_Renderer->draw();

		// Swap the buffers so that what we drew will appear on the screen.
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}

	// delete objects
	delete coordinate_system_renderer;
	delete Terrain_Renderer;
	delete Terrain_Renderer1;
	delete Terrain_Renderer2;
	delete Terrain_Renderer3;
	delete TerrainRiv_Renderer1;
	delete TerrainRiv_Renderer2;
	delete Skybox_Renderer;
	delete treeObj;
	while (!treeVector.empty())
	{
		treeVector.pop_back();
	}

	delete terraintexture;
	delete terraintexture1;
	delete apperance_terrain;
	delete apperance_terrain1;
	delete treetexture;
	delete cubmaptexture;
	delete apperance_tree;
	delete apperance_skybox;
	delete &Directional_light_source;
	delete &material_0;

	return 1;
}

