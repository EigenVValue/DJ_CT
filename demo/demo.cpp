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
#include "shader.hpp"
//#include "loadOBJ.hpp"
//#include "controls.hpp"
#include "controlsForFOV.hpp"
#include "getNormals.hpp"
#include "texture.hpp"
#include "getUVs.hpp"

// Include dcmToModel
#include "dependencies/include/converttobmp.h"
#include "getImageData.hpp"
#include "dcmToModel.hpp"

// Set window width and height
const GLuint  WIDTH = 1024;
const GLuint  HEIGHT = 768;
const char* PATH // DCM path
= "D:\\VS\\Project\\DJ_medical\\CT_img\\Recon_4";
//= "D:\\VS\\Project\\DJ_medical\\CT_img\\CT-head\\CT-head\\LiZhanYou\\20200211153157\\1"; // error
//= "D:\\VS\\Project\\DJ_medical\\CT_img\\Wu Li Juan\\Wu Li Juan\\20180910092809.000\\3";
const uint8_t ISO = 204;	// Isosurface
const uint8_t THRESHOLD = 225;	// Threshold

// MVP variables
mat4 RotationMatrix = mat4(1);
glm::vec3 position = glm::vec3(0, 0, -15);
glm::vec3 up = glm::vec3(0, 2, -15);
glm::vec3 rotX = glm::vec3(1, 0, 0);
glm::vec3 rotY = glm::vec3(0, 1, 0);

// Convert dcm files to obj model
void dcmFileToModel(
	const char* path,
	const uint8_t iso,
	const uint8_t threshold,
	std::vector<glm::vec3> & objVertices,
	std::vector<unsigned int> & objFaces,
	std::vector<int> & colors,
	int &rescale_intercept,
	unsigned short &rescale_slope
) {
	// Set x, y, z and raw data
	unsigned int dimX = 0;
	unsigned int dimY = 0;
	unsigned int dimZ = 0;
	std::vector<uint8_t> raw;
	// Set parameter to convert Grayscale to CT number
	rescale_intercept = 0;
	rescale_slope = 0;

	// Convert dcm files to raw file
	getImageData(
		path,
		raw,
		dimX,
		dimY,
		dimZ,
		rescale_intercept,
		rescale_slope
	);
	removeNoise(
		raw,
		dimX,
		dimY,
		dimZ,
		threshold
	);
	printf("%s", "Get image done.\n");

	// Convert raw file to obj model
	// Use Marching Cubes Algorithm
	dcmToModel dcm2Model;
	dcm2Model.run(
		raw,
		dimX,
		dimY,
		dimZ,
		iso,
		objVertices,
		objFaces,
		colors,
		rescale_intercept,
		rescale_slope
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

	// Set background color
	glClearColor(0.467f, 0.467f, 0.467f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint depthProgramID = LoadShaders("DepthRTT.vertexshader", "DepthRTT.fragmentshader");
	GLuint depthProgramID2 = LoadShaders("DepthRTT.vertexshader", "DepthRTT.fragmentshader");
	GLuint depthProgramID3 = LoadShaders("DepthRTT.vertexshader", "DepthRTT.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint depthMatrixID = glGetUniformLocation(depthProgramID, "depthMVP");
	GLuint depthMatrixID2 = glGetUniformLocation(depthProgramID2, "depthMVP");
	GLuint depthMatrixID3 = glGetUniformLocation(depthProgramID3, "depthMVP");

	// Load the texture
	GLuint Texture;
	{
		std::filesystem::path currPath = argv[0];
		currPath = currPath.parent_path();
		currPath += "\\img\\Texture.bmp";
		char* path = currPath.string().data();
		Texture = loadBMP(path);
	}

	// Set vertex, color and normal
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> faces;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<int> colors;

	// Get vertex via loading raw file
	int rescale_intercept;
	unsigned short rescale_slope;
	dcmFileToModel(PATH, ISO, THRESHOLD, vertices, faces, colors, rescale_intercept, rescale_slope);

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

	// Get pivot (Need change)
	{
		float pivot[3] = { 0.0f };
		for (auto & vertex : vertices) {
			// Add up
			pivot[0] += vertex.x;
			pivot[1] += vertex.y;
			pivot[2] += vertex.z;
		}
		pivot[0] /= vertices.size();
		pivot[1] /= vertices.size();
		pivot[2] /= vertices.size();

		for (auto & vertex : vertices) {
			vertex.x -= pivot[0];
			vertex.y -= pivot[1];
			vertex.z -= pivot[2];
		}
	}

	// Get normal
	// Surface normal vector
	//normals = getNormals(vertices);
	// Vertex normal vector
	normals = getVertexNormals(vertices, faces);

	int uvThreshold = THRESHOLD * rescale_slope + ((float)rescale_intercept / 4096.0f * 255.0f);
	getUVs(vertices, colors, uvs, uvThreshold);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(unsigned int), &faces[0], GL_STATIC_DRAW);

	// ----------------------------
	// Render to Texture
	// ----------------------------
	// The framebuffer, which regroups 3 textures.
	GLuint FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	GLuint depthTexture;
	{
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	}

	GLuint FramebufferName2 = 0;
	glGenFramebuffers(1, &FramebufferName2);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName2);
	GLuint depthTexture2;
	{
		glGenTextures(1, &depthTexture2);
		glBindTexture(GL_TEXTURE_2D, depthTexture2);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture2, 0);
	}

	GLuint FramebufferName3 = 0;
	glGenFramebuffers(1, &FramebufferName3);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName3);
	GLuint depthTexture3;
	{
		glGenTextures(1, &depthTexture3);
		glBindTexture(GL_TEXTURE_2D, depthTexture3);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture3, 0);
	}


	// No color output in the bound framebuffer, only depth.
	glDrawBuffer(GL_NONE);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) return false;

	// Create and compile GLSL program from the shaders
	GLuint programID = LoadShaders("vShader.vertexshader", "fShader.fragmentshader");

	// Get a handle for "textureSampler" uniform
	GLuint TextureID = glGetUniformLocation(programID, "textureSampler");

	// Get a handle for "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Shadow map uniform
	GLuint DepthBiasID = glGetUniformLocation(programID, "DepthBiasMVP");
	GLuint ShadowMapID = glGetUniformLocation(programID, "shadowMap");
	GLuint DepthBiasID2 = glGetUniformLocation(programID, "DepthBiasMVP2");
	GLuint ShadowMapID2 = glGetUniformLocation(programID, "shadowMap2");
	GLuint DepthBiasID3 = glGetUniformLocation(programID, "DepthBiasMVP3");
	GLuint ShadowMapID3 = glGetUniformLocation(programID, "shadowMap3");

	// Get a handle for "LightPosition" uniform
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	GLuint LightID2 = glGetUniformLocation(programID, "LightPosition_worldspace2");
	GLuint LightID3 = glGetUniformLocation(programID, "LightPosition_worldspace3");

	printf("Start rendering\n");
	end = clock();
	printf("%f\n", (float)(end - start) / CLOCKS_PER_SEC);

	// Start rendering
	do {
		// Set light postion for 3 three buffer render
		// Side light
		float sideZ = -11.0f;
		float sideX = sideZ * sqrt(3.0f);
		glm::vec3 lightPos = vec3(sideX, 0.0f, sideZ);
		glm::vec3 lightPos2 = vec3(-sideX, 0.0f, sideZ);
		// Back light
		glm::vec3 lightPos3 = vec3(0.0f, 3.0f, 15.0f);

		// Render to our framebuffer

		// ----------------------------
		// First light
		// ----------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		glViewport(0, 0, 1024, 1024); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		// We don't use bias in the shader, but instead we draw back faces, 
		// which are already separated from the front faces by a small distance 
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(depthProgramID);

		glm::mat4 depthProjectionMatrix = glm::ortho<float>(-20, 20, -20, 20, -20, 20);
		glm::mat4 depthViewMatrix = glm::lookAt(lightPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glm::mat4 depthModelMatrix = glm::mat4(1.0f);
		glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,								 // The attribute we want to configure
			3,								 // size
			GL_FLOAT,			     // type
			GL_FALSE,			     // normalized?
			0,							     // stride
			(void*)0					 // array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,		// mode
			faces.size(),					// count
			GL_UNSIGNED_INT,	// type
			(void*)0						// element array buffer offset
		);

		glDisableVertexAttribArray(0);

		// ----------------------------------------------
		// Second light (only light changes)
		// ----------------------------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName2);
		glViewport(0, 0, 1024, 1024);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(depthProgramID2);

		depthProjectionMatrix = glm::ortho<float>(-20, 20, -20, 20, -20, 20);
		depthViewMatrix = glm::lookAt(lightPos2, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		depthModelMatrix = glm::mat4(1.0f);
		glm::mat4 depthMVP2 = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

		glUniformMatrix4fv(depthMatrixID2, 1, GL_FALSE, &depthMVP2[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		glDrawElements(
			GL_TRIANGLES,
			faces.size(),
			GL_UNSIGNED_INT,
			(void*)0
		);

		glDisableVertexAttribArray(0);

		// -----------------------------------------------------------
		// Third light (back light, only light changes)
		// -----------------------------------------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName2);
		glViewport(0, 0, 1024, 1024);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(depthProgramID2);

		depthProjectionMatrix = glm::ortho<float>(-20, 20, -20, 20, -20, 20);
		depthViewMatrix = glm::lookAt(lightPos3, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		depthModelMatrix = glm::mat4(1.0f);
		glm::mat4 depthMVP3 = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

		glUniformMatrix4fv(depthMatrixID3, 1, GL_FALSE, &depthMVP3[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		glDrawElements(
			GL_TRIANGLES,
			faces.size(),
			GL_UNSIGNED_INT,
			(void*)0
		);

		glDisableVertexAttribArray(0);

		// ----------------------------
		// Render to the screen
		// ----------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, WIDTH, HEIGHT); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use shader
		glUseProgram(programID);

		// Build MVP Matrix
		// ProjectionMatrix & ViewMatrix
		computeMatricesFromInputs(WIDTH, HEIGHT, position, up, rotX, rotY);
		mat4 ProjectionMatrix = getProjectionMatrix();
		mat4 ViewMatrix = getViewMatrix();

		// ModelMatrix
		vec3 modelRotation = getModelRotation();
		// Quaternion is better than Euler Angle
		RotationMatrix = eulerAngleYXZ(modelRotation.y, modelRotation.x, modelRotation.z) * RotationMatrix;
		mat4 TranslationMatrix = translate(mat4(1.0f), getModelPosition());
		mat4 ScalingMatrix = scale(mat4(1.0f), getModelScaling());
		mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;

		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Get depth matrix
		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);
		glm::mat4 depthBiasMVP = biasMatrix * depthMVP;
		glm::mat4 depthBiasMVP2 = biasMatrix * depthMVP2;
		glm::mat4 depthBiasMVP3 = biasMatrix * depthMVP3;

		// Send transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
		// in the "Depth" uniform
		glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);
		glUniformMatrix4fv(DepthBiasID2, 1, GL_FALSE, &depthBiasMVP2[0][0]);
		glUniformMatrix4fv(DepthBiasID3, 1, GL_FALSE, &depthBiasMVP3[0][0]);
		// in the "Light" uniform
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(LightID2, lightPos2.x, lightPos2.y, lightPos2.z);
		glUniform3f(LightID3, lightPos3.x, lightPos3.y, lightPos3.z);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "textureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);

		// First light
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glUniform1i(ShadowMapID, 1);

		// Second light
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, depthTexture2);
		glUniform1i(ShadowMapID2, 2);

		// Third light
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthTexture3);
		glUniform1i(ShadowMapID2, 3);

		// 1rst attribute buffer : Vertices
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
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,									// attribute. No particular reason for 1, but must match the layout in the shader.
			2,									// size : U+V => 2
			GL_FLOAT,					// type
			GL_FALSE,					// normalized?
			0,									// stride
			(void*)0						// array buffer offset
		);

		// 3rd attribute buffer : Normals
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
			faces.size(),					// count
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
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);

	glDeleteProgram(programID);
	glDeleteProgram(depthProgramID);
	glDeleteProgram(depthProgramID2);
	glDeleteProgram(depthProgramID3);

	glDeleteFramebuffers(1, &FramebufferName);
	glDeleteFramebuffers(1, &FramebufferName2);
	glDeleteFramebuffers(1, &FramebufferName3);

	glDeleteTextures(1, &Texture);
	glDeleteTextures(1, &depthTexture);
	glDeleteTextures(1, &depthTexture2);
	glDeleteTextures(1, &depthTexture3);

	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}