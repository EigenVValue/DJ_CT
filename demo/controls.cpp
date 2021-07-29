/*

// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; 

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
// Initial position :
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

// Initial horizontal angle : 
float horizontalAngle = 0.01f;
// Initial vertical angle :
float verticalAngle = 0.01f;

float speed = 3.0f; // 3 units per second
float mouseSpeed = 0.001f;
bool click = false;

void computeMatricesFromInputs(const int WIDTH, const int HEIGHT) {

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);

	// Tow mouse event
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);

	// Rotate
	if (click) {
		// Compute new orientation
		horizontalAngle -= mouseSpeed * float(WIDTH / 2 - xpos);
		verticalAngle -= mouseSpeed * float(HEIGHT / 2 - ypos);
		modelRotation = glm::vec3(3.14159f / 2.0f * verticalAngle, 3.14159f / 2.0f * horizontalAngle, 0);
	}
	// Move up
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		modelPosition -= glm::vec3(0, 1,0) * deltaTime * speed;
	}
	// Move down
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		modelPosition += glm::vec3(0, 1, 0) * deltaTime * speed;
	}
	// Move right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		modelPosition -= glm::vec3(1, 0, 0) * deltaTime * speed;
	}
	// Move left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		modelPosition += glm::vec3(1, 0, 0) * deltaTime * speed;
	}

	// Projection matrix : 45 angle, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(45.0f), float(WIDTH) / float(HEIGHT), 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		glm::vec3(5, 10, 10),	// Camera is here
		glm::vec3(0,0,0),			// and looks at the origin
		glm::vec3(0,1,0)			// Head is up (set to 0,-1,0 to look upside-down)
	);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		click = true;
	} else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		click = false;
	}
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
	if (modelScaling.x + yoffset * 0.0005 > 0.0f) { // Set minimum scale coefficient
		modelScaling += yoffset * 0.0005;
	}
}

*/