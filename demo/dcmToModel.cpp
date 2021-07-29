// Include standard liabraries
#include <vector>

// Dual mc builder
#include "dualmc.h"
#include "dualmc.hpp"

// GLM
#include "glm/glm.hpp"

#include "dcmToModel.hpp"

#include <time.h>

void dcmToModel::run(
	const std::vector<uint8_t> raw,
	const unsigned int &dimX,
	const unsigned int &dimY,
	const unsigned int &dimZ,
	const uint8_t iso,
	std::vector<glm::vec3> & objVertices,
	std::vector<unsigned int> & objFaces,
	std::vector<int> & colors,
	const int & rescale_intercept,
	const unsigned short & rescale_slope
) {
	// Set volume
	Volume volume;
	// Load raw file
	volume.dimX = dimX;
	volume.dimY = dimY;
	volume.dimZ = dimZ;
	volume.iso = iso;
	volume.data.resize(dimX * dimY * dimZ);
	volume.data = raw;

	// Array of vertices for the extracted surface
	std::vector<dualmc::Vertex> vertices;

	// Array of quad indices for the extracted surface
	std::vector<dualmc::Quad> quads;

	// Array of colors for the extracted surface
	std::vector<uint8_t> objColors;

	// Compute surface
	computeSurface(volume, vertices, quads, objColors);

	// TODO:
	// Code below is a template way
	// Need to improve

	// Get vertices
	for (auto const & v : vertices) {
		glm::vec3 vertex;
		vertex.x = v.x;
		vertex.y = v.y;
		vertex.z = v.z;
		objVertices.push_back(vertex);
	}

	// Get quad indices
	for (auto const & q : quads) {
		unsigned int face[4];
		face[0] = q.i0;
		face[1] = q.i1;
		face[2] = q.i2;
		face[3] = q.i3;
		objFaces.push_back(face[0]);
		objFaces.push_back(face[1]);
		objFaces.push_back(face[2]);

		objFaces.push_back(face[0]);
		objFaces.push_back(face[2]);
		objFaces.push_back(face[3]);
	}

	// Get colors (CT number)
	for (auto color : objColors)
	{
		// Hu = pixel * slope + intercept
		int newColor = color * rescale_slope + ((float)rescale_intercept/4096.0f * 255.0f);
		//int newColor = color;
		colors.push_back(newColor);
	}
}

void dcmToModel::computeSurface(
	Volume & volume,
	std::vector<dualmc::Vertex> & vertices,
	std::vector<dualmc::Quad> & quads,
	std::vector<uint8_t> & colors
) {
	printf("%s" ,"Computing surface...\n");
	dualmc builder;
	builder.build(
		&volume.data.front(),
		volume.dimX,
		volume.dimY,
		volume.dimZ,
		volume.iso,
		vertices,
		quads,
		colors
	);
	printf("%s", "Computing surface done.\n");
}