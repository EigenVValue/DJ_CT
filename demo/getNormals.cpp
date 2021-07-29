#include <vector>
//#include <unordered_map>

#include <glm/glm.hpp>

#include "getNormals.hpp"

std::vector<glm::vec3> getVertexNormals(
	const std::vector<glm::vec3> &objVertices,
	const std::vector<unsigned int> &objFaces)
{
	std::vector<glm::vec3> normals;
	// Use an array to save vertex normal data
	glm::vec3 *faceArr = new glm::vec3[objVertices.size()];
	memset(faceArr, 0, sizeof(glm::vec3) * objVertices.size());

	// Add surface normal vector
	for (int i = 0; i < objFaces.size(); i += 3) {

		// First triangle
		glm::vec3 faceNorm = getNormal(
			objVertices[objFaces.at(i)],
			objVertices[objFaces.at(i + 1)],
			objVertices[objFaces.at(i + 2)])
			* getArea(
				objVertices[objFaces.at(i)],
				objVertices[objFaces.at(i + 1)],
				objVertices[objFaces.at(i + 2)]);

		// Vertex 1
		faceArr[objFaces.at(i)] += faceNorm;
		// Vertex 2
		faceArr[objFaces.at(i+1)] += faceNorm;
		// Vertex 3
		faceArr[objFaces.at(i+2)] += faceNorm;

	}

	// Build normals
	for (int i = 0; i < objVertices.size(); i++)
	{
		glm::vec3 normal = faceArr[i];
		normal = glm::normalize(normal);
		normals.push_back(normal);
	}

	delete faceArr;

	return normals;
}


// Get surface normal vector.
// It seems we do not need to use this function.
std::vector<glm::vec3> getNormals(
	const std::vector<glm::vec3> &vertices)
{
	std::vector<glm::vec3> normals;
	// Start at 3 points which is one triangle.
	for (int i = 0; i < vertices.size(); i+=3)
	{
		glm::vec3 normal = getNormal(vertices.at(i),
			vertices.at(i + 1), vertices.at(i + 2));

		// Let the normal be every point of this triangle.
		normals.push_back(normal);
		normals.push_back(normal);
		normals.push_back(normal);
	}
	return normals;
}

glm::vec3 getNormal(
	const glm::vec3 &vertex1,
	const glm::vec3 &vertex2, 
	const glm::vec3 &vertex3
) {
	glm::vec3 normal;
	// U = p2 - p1
	// V = p3 - p1
	// N = U X V
	glm::vec3 vecU = vertex1 - vertex2;
	glm::vec3 vecV = vertex1 - vertex3;
	normal = glm::cross(vecU, vecV);

	return normal;
}

/*
**	This is function using hash table.
**	It is slower than function using array

 glm::vec3 getNormal(const glm::vec3 &vertex1,
	const glm::vec3 &vertex2, const glm::vec3 &vertex3)
{
	glm::vec3 normal;
	// U = p2 - p1
	// V = p3 - p1
	// N = U X V
	glm::vec3 vecU = vertex1 - vertex2;
	glm::vec3 vecV = vertex1 - vertex3;
	normal = glm::cross(vecU, vecV);
	

	return normal;
}

std::vector<glm::vec3> getVertexNormals(
	const std::vector<glm::vec3> &objVertices,
	const std::vector<unsigned int> &objFaces)
{
	std::vector<glm::vec3> normals;
	std::unordered_map<int, glm::vec3> hashTable;

	// Hashing
	for (int i = 0; i < objFaces.size(); i+=3) {
		auto faceNorm = getNormal(
				objVertices.at(objFaces .at(i)-1),
				objVertices.at(objFaces.at(i+1) -1),
				objVertices.at(objFaces.at(i+2) -1))
			* getArea(
				objVertices.at(objFaces.at(i) -1),
				objVertices.at(objFaces.at(i+1) -1),
				objVertices.at(objFaces.at(i+2) -1));

		// Vertex 1
		if (hashTable.count(objFaces.at(i)) == 0) { // If not exist
			hashTable[objFaces.at(i)] = faceNorm;
		} else { // If already exist
			hashTable[objFaces.at(i)] += faceNorm;
		}
		// Vertex 2
		if (hashTable.count(objFaces.at(i+1)) == 0) { // If not exist
			hashTable[objFaces.at(i+1)] = faceNorm;
		}
		else { // If already exist
			hashTable[objFaces.at(i+1)] += faceNorm;
		}
		// Vertex 3
		if (hashTable.count(objFaces.at(i+2)) == 0) { // If not exist
			hashTable[objFaces.at(i+2)] = faceNorm;
		}
		else { // If already exist
			hashTable[objFaces.at(i+2)] += faceNorm;
		}
	}

	// Build normals
	for (int i = 0; i < objFaces.size(); i++)
	{
		glm::vec3 normal = hashTable[objFaces.at(i)];
		normal = glm::normalize(normal);
		normals.push_back(normal);
	}

	return normals;
}
*/

// Use Heron's formula to calculate area of triangle
float getArea(glm::vec3 vec1, glm::vec3 vec2, glm::vec3 vec3) {
	// Heron's formula:
	// L = (a+b+c)/2
	// S = sqr(L*(L-a)*(L-b)*(L-c)

	float len1 = glm::length(vec1-vec2);
	float len2 = glm::length(vec2-vec3);
	float len3 = glm::length(vec1-vec3);
	float l = len1 + len2 + len3;
	l /= 2;
	float s = glm::sqrt(l*(l - len1)*(l - len2)*(l - len3));

	return s;
}