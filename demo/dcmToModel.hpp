#ifndef DCMTOMODEL_HPP
#define DCMTOMODEL_HPP

#include "dualmc.h"

class dcmToModel {
public:
	void run(
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
	);

	// Volume to save raw data
	struct Volume {
		int32_t dimX;
		int32_t dimY;
		int32_t dimZ;
		uint8_t iso;
		std::vector<uint8_t> data;
	};

private:
	void computeSurface(
		Volume & volume,
		std::vector<dualmc::Vertex> & vertices,
		std::vector<dualmc::Quad> & quads,
		std::vector<uint8_t> & colors
	);
};

#endif //DCMTOMODEL_HPP