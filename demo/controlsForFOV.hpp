#ifndef CONTROLSFORFOV_HPP
#define CONTROLSFORFOV_HPP

void computeMatricesFromInputs(
	const int WIDTH, const int HEIGHT,
	glm::vec3 &position, glm::vec3 & up,
	glm::vec3 &x, glm::vec3 &y
);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::vec3 getModelPosition();
glm::vec3 getModelRotation();
glm::vec3 getModelScaling();

void mouse_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

#endif // CONTROLSFORFOV_HPP