// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; 

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "controlsForFOV.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
// Model Matrix; constant
glm::vec3 modelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 modelRotation = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 modelScaling = glm::vec3(0.03f, 0.03f, 0.03f);

glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}
glm::vec3 getModelPosition() {
	return modelPosition;
}
glm::vec3 getModelRotation() {
	return modelRotation;
}
glm::vec3 getModelScaling() {
	return modelScaling;
}

// Initial position & up:

// Initial horizontal angle : none
float horizontalAngle = 0.0f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float mouseSpeed = 0.005f;
bool click = false;
double offset = 0;

double xpos, ypos;
double currXpos, currYpos;

void computeMatricesFromInputs(
	const int WIDTH, const int HEIGHT,
	glm::vec3 &position, glm::vec3 & up,
	glm::vec3 &rotx, glm::vec3 &roty
) {
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);

	// Scroll
	{
		// Change position
		position.x *= 1 - offset * 0.05;
		position.y *= 1 - offset * 0.05;
		position.z *= 1 - offset * 0.05;
		up.x *= 1 - offset * 0.05;
		up.y *= 1 - offset * 0.05;
		up.z *= 1 - offset * 0.05;
		// Reset offset
		offset = 0;
	}

	// Rotate
	if (click) {
		// Get mouse position
		glfwGetCursorPos(window, &currXpos, &currYpos);
		// Compute new orientation
		horizontalAngle =mouseSpeed * float(xpos - currXpos);
		verticalAngle = -mouseSpeed * float(ypos - currYpos);
		xpos = currXpos;
		ypos = currYpos;

		// Rotate model, do what you like
		/*
		// Rotate at X-aixs
		modelRotation.x = verticalAngle;
		// Rotate at Y-aixs
		modelRotation.y = horizontalAngle;
		*/
		
		// Rotate postion
		// Using Rodrigues' rotation formula:
		//		v' = vcos(theta) + dot(u,v)u(1-cos(theta)) + cross(u,v)sin(theta)
		// where v' is the vector we want, v is origin vector, u is a unit vector
		// Rotate at Y-aixs
		position = position * cos(horizontalAngle) 
			+ dot(roty, position) * roty * (1 - cos(horizontalAngle)) 
			+ glm::cross(roty, position) * sin(horizontalAngle);
		up = up * cos(horizontalAngle)
			+ dot(roty, up) * roty * (1 - cos(horizontalAngle))
			+ glm::cross(roty, up) * sin(horizontalAngle);
		rotx = rotx * cos(horizontalAngle)
			+ dot(roty, rotx) * roty * (1 - cos(horizontalAngle))
			+ glm::cross(roty, rotx) * sin(horizontalAngle);
		rotx = glm::normalize(rotx);
		// Rotate at X-aixs
		position = position * cos(verticalAngle)
			+ dot(rotx, position) * rotx * (1 - cos(verticalAngle))
			+ glm::cross(rotx, position) * sin(verticalAngle);
		up = up * cos(verticalAngle)
			+ dot(rotx, up) * rotx * (1 - cos(verticalAngle))
			+ glm::cross(rotx, up) * sin(verticalAngle);
		roty = roty * cos(verticalAngle)
			+ dot(rotx, roty) * rotx * (1 - cos(verticalAngle))
			+ glm::cross(rotx, roty) * sin(verticalAngle);
		roty = glm::normalize(roty);
	}

	// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(45.0f), float(WIDTH) / float(HEIGHT), 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		glm::vec3(position.x, position.y, position.z),	// Camera is here
		glm::vec3(0, 0, 0),											// Looks at origin(0,0,0)
		glm::vec3(up.x, up.y, up.z)								// Head is up (set to 0,-1,0 to look upside-down)
	);
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		click = true;
		glfwGetCursorPos(window, &xpos, &ypos);
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		click = false;
		xpos = NULL;
		ypos = NULL;
	}
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
	offset = yoffset;
}