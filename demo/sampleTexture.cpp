/*
// Include standard liabraries
#include <vector>
#include <filesystem>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;

// Include common
#include <shader.hpp>
#include <loadOBJ.hpp>
//#include <controls.hpp>
#include <controlsForFOV.hpp>
#include <getNormals.hpp>
#include <texture.hpp>
#include <getUVs.hpp>

// Include dcmToModel
#include "dependencies/include/converttobmp.h"
#include "getImageData.h"
#include "dcmToModel.hpp"

// Set window width and height
const GLuint  WIDTH = 1024;
const GLuint  HEIGHT = 768;
const char* PATH // DCM path
	= "D:\\VS\\Project\\DJ_medical\\CT_img\\Recon_4";
const float ISO = 0.8f;	// Threshold

// Convert dcm files to obj model
void dcmFileToModel(
	const char* path,
	const float iso,
	std::vector<glm::vec3> & objVertices,
	std::vector<unsigned int> & objFaces
) {
	// Set x, y, z and data
	unsigned int dimX=0;
	unsigned int dimY=0;
	unsigned int dimZ=0;
	std::vector<uint8_t> raw;

	// Convert dcm files to raw file
	getImageData(
		path,
		raw,
		dimX,
		dimY,
		dimZ
	);
	printf("%s", "Get image done.\n");

	// Convert raw file to obj model
	dcmToModel dcm2Model;
	dcm2Model.run(
		raw,
		dimX,
		dimY,
		dimZ,
		iso,
		objVertices,
		objFaces
	);
}

// MAIN function
int main(int argc, char* argv[]) {
	clock_t start, end;
	start = clock();
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_SAMPLES, 16);
	glEnable(GL_MULTISAMPLE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(WIDTH, HEIGHT, "demo", nullptr, nullptr);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	glewInit();
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set background color
	glClearColor(0.467f, 0.467f, 0.467f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
	// Shade model
	glShadeModel(GL_SMOOTH);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	// Set vertex, color and normal
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	// Get vertex via loading raw file
	std::vector<glm::vec3> objVertices;
	std::vector<unsigned int> objFaces;
	dcmFileToModel(PATH, ISO, objVertices, objFaces);

	// Load obj no need for now
	/*
	{
		std::filesystem::path currPath = argv[0];
		currPath = currPath.parent_path();
		//currPath += "\\img\\cube.obj";
		//currPath += "\\img\\head.obj";
		//currPath += "\\img\\head2.obj";
		currPath += "\\img\\bone2.obj";
		char* path = currPath.string().data();
		bool res = loadOBJ(path, objVertices, objFaces);
		if (!res) {
			printf("loadOBJ fail!");
			return 0;
		}
	}*/

	// Get vertex
	//objVerticesToGLVertices(vertices, objVertices, objFaces);
	{
		// Get pivot Need change
		float pivot[3] = { 0.0f };
		for (auto & vertex : objVertices) {
			// Add up
			pivot[0] += vertex.x;
			pivot[1] += vertex.y;
			pivot[2] += vertex.z;
		}
		pivot[0] /= objVertices.size();
		pivot[1] /= objVertices.size();
		pivot[2] /= objVertices.size();

		for (auto & vertex : objVertices) {
			vertex.x -= pivot[0];
			vertex.y -= pivot[1];
			vertex.z -= pivot[2];
		}
	}
	vertices = objVertices;

	// Get normal
	// Surface normal vector
	//normals = getNormals(vertices);
	// Vertex normal vector
	normals = getVertexNormals(objVertices, objFaces);

	/*getUVs(vertices, vec3(1, 0, 0), uvs);*/

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	/*GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);*/
	
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, objFaces.size() * sizeof(unsigned int), &objFaces[0], GL_STATIC_DRAW);

	// Create and compile GLSL program from the shaders
	GLuint programID = LoadShaders("vShader.vertexshader", "fShader.fragmentshader");

	// Get a handle for "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Get a handle for "LightPosition" uniform
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	GLuint LightID2 = glGetUniformLocation(programID, "LightPosition_worldspace2");
	GLuint LightID3 = glGetUniformLocation(programID, "LightPosition_worldspace3");

	// Load DDS
	GLuint Texture;
	{
		std::filesystem::path currPath = argv[0];
		currPath = currPath.parent_path();
		currPath += "\\img\\uvmap.DDS";
		char* path = currPath.string().data();
		Texture = loadDDS(path);
	}
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	printf("Start rendering\n");
	end = clock();
	printf("%f", (float)(end-start) / CLOCKS_PER_SEC);
	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use shader
		glUseProgram(programID);

		// Build MVP Matrix
		// ProjectionMatrix & ViewMatrix
		computeMatricesFromInputs(WIDTH, HEIGHT);
		mat4 ProjectionMatrix = getProjectionMatrix();
		mat4 ViewMatrix = getViewMatrix();

		// ModelMatrix
		vec3 modelRotation = getModelRotation();
		// Quaternion is better than Euler Angle
		mat4 RotationMatrix = eulerAngleYXZ(modelRotation.y, modelRotation.x, modelRotation.z);
		mat4 TranslationMatrix = translate(mat4(1.0f), getModelPosition());
		mat4 ScalingMatrix = scale(mat4(1.0f), getModelScaling());
		mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;

		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		// Set light postion
		// Side light
		float sideZ = -11.0f;
		float sideX = sideZ * sqrt(3.0f);
		glm::vec3 lightPos = vec3(sideX, 0.0f, sideZ);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glm::vec3 lightPos2 = vec3(-sideX, 0.0f, sideZ);
		glUniform3f(LightID2, lightPos2.x, lightPos2.y, lightPos2.z);
		// Back light
		glm::vec3 lightPos3 = vec3(0.0f, 3.0f, 15.0f);
		glUniform3f(LightID3, lightPos3.x, lightPos3.y, lightPos3.z);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,									// attribute. No particular reason for 0, but must match the layout in the shader.
			3,									// size
			GL_FLOAT,					// type
			GL_FALSE,					// normalized?
			0,									// stride
			(void*)0						// array buffer offset
		);
		
		// 2nd attribute buffer : UVs
		//glEnableVertexAttribArray(1);
		//glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		//glVertexAttribPointer(
		//	1,									// attribute. No particular reason for 1, but must match the layout in the shader.
		//	2,									// size : U+V => 2
		//	GL_FLOAT,					// type
		//	GL_FALSE,					// normalized?
		//	0,									// stride
		//	(void*)0						// array buffer offset
		//);
		
		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			2,									// attribute
			3,									// size
			GL_FLOAT,					// type
			GL_FALSE,					// normalized?
			0,									// stride
			(void*)0						// array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		
		glDrawElements(
			GL_TRIANGLES,		// mode
			objFaces.size(),			// count
			GL_UNSIGNED_INT,	// type
			(void*)0						// element array buffer offset
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	/*glDeleteBuffers(1, &uvbuffer);*/
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
*/