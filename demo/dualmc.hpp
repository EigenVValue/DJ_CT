/// Copyright (C) 2017, Dominik Wodniok
/// This software may be modified and distributed under the terms
/// of the BSD 3-Clause license. See the LICENSE.txt file for details.

/// \file   dualmc.tpp
/// \author Dominik Wodniok
/// \date   2009

#include "dualmc.h"

///------------------------------------------------------------------------------

void dualmc::build(
	const uint8_t * data,
	const int32_t dimX,
	const int32_t dimY,
	const int32_t dimZ,
	const uint8_t iso,
	std::vector<dualmc::Vertex> & vertices,
	std::vector<dualmc::Quad> & quads,
	std::vector<uint8_t> & colors
) {

	/// set members
	this->dims[0] = dimX;
	this->dims[1] = dimY;
	this->dims[2] = dimZ;
	this->data = data;

	/// clear vertices and quad indices
	vertices.clear();
	quads.clear();

	buildSharedVerticesQuads(iso, vertices, quads, colors);
}

///------------------------------------------------------------------------------

void dualmc::buildSharedVerticesQuads(
	uint8_t const iso,
	std::vector<dualmc::Vertex> & vertices,
	std::vector<dualmc::Quad> & quads,
	std::vector<uint8_t> & colors
) {
	int32_t const reducedX = dims[0] - 2;
	int32_t const reducedY = dims[1] - 2;
	int32_t const reducedZ = dims[2] - 2;

	int32_t i0, i1, i2, i3;

	pointToIndex.clear();

	/// iterate voxels
	for (int32_t z = 0; z < reducedZ; ++z)
		for (int32_t y = 0; y < reducedY; ++y)
			for (int32_t x = 0; x < reducedX; ++x) {
				/// construct quads for x edge
				if (z > 0 && y > 0) {
					bool const entering = data[gA(x, y, z)] < iso && data[gA(x + 1, y, z)] >= iso;
					bool const exiting = data[gA(x, y, z)] >= iso && data[gA(x + 1, y, z)] < iso;
					if (entering || exiting) {
						/// generate quad
						i0 = getSharedDualPointIndex(x, y, z, iso, EDGE0, vertices, colors);
						i1 = getSharedDualPointIndex(x, y, z - 1, iso, EDGE2, vertices, colors);
						i2 = getSharedDualPointIndex(x, y - 1, z - 1, iso, EDGE6, vertices, colors);
						i3 = getSharedDualPointIndex(x, y - 1, z, iso, EDGE4, vertices, colors);

						if (entering) {
							quads.emplace_back(i0, i1, i2, i3);
						}
						else {
							quads.emplace_back(i0, i3, i2, i1);
						}
					}
				}

				/// construct quads for y edge
				if (z > 0 && x > 0) {
					bool const entering = data[gA(x, y, z)] < iso && data[gA(x, y + 1, z)] >= iso;
					bool const exiting = data[gA(x, y, z)] >= iso && data[gA(x, y + 1, z)] < iso;
					if (entering || exiting) {
						/// generate quad
						i0 = getSharedDualPointIndex(x, y, z, iso, EDGE8, vertices, colors);
						i1 = getSharedDualPointIndex(x, y, z - 1, iso, EDGE11, vertices, colors);
						i2 = getSharedDualPointIndex(x - 1, y, z - 1, iso, EDGE10, vertices, colors);
						i3 = getSharedDualPointIndex(x - 1, y, z, iso, EDGE9, vertices, colors);

						if (exiting) {
							quads.emplace_back(i0, i1, i2, i3);
						}
						else {
							quads.emplace_back(i0, i3, i2, i1);
						}
					}
				}

				/// construct quads for z edge
				if (x > 0 && y > 0) {
					bool const entering = data[gA(x, y, z)] < iso && data[gA(x, y, z + 1)] >= iso;
					bool const exiting = data[gA(x, y, z)] >= iso && data[gA(x, y, z + 1)] < iso;
					if (entering || exiting) {
						/// generate quad
						i0 = getSharedDualPointIndex(x, y, z, iso, EDGE3, vertices, colors);
						i1 = getSharedDualPointIndex(x - 1, y, z, iso, EDGE1, vertices, colors);
						i2 = getSharedDualPointIndex(x - 1, y - 1, z, iso, EDGE5, vertices, colors);
						i3 = getSharedDualPointIndex(x, y - 1, z, iso, EDGE7, vertices, colors);

						if (exiting) {
							quads.emplace_back(i0, i1, i2, i3);
						}
						else {
							quads.emplace_back(i0, i3, i2, i1);
						}
					}
				}
			}
}

///------------------------------------------------------------------------------

int32_t dualmc::getSharedDualPointIndex(
	const int32_t cx,
	const int32_t cy,
	const int32_t cz,
	const uint8_t iso,
	const DMCEdgeCode edge,
	std::vector<Vertex> & vertices,
	std::vector<uint8_t> & colors
) {
	/// create a key for the dual point from its linearized cell ID and point code
	DualPointKey key;
	key.linearizedCellID = gA(cx, cy, cz);
	key.pointCode = getDualPointCode(cx, cy, cz, iso, edge);

	/// have we already computed the dual point?
	// pointToIndex -> hash table
	auto iterator = pointToIndex.find(key);
	if (iterator != pointToIndex.end()) {
		/// just return the dual point index
		return iterator->second;
	}
	else {
		/// create new vertex and vertex id
		int32_t newVertexId = vertices.size();
		vertices.emplace_back();
		colors.emplace_back();
		calculateDualPoint(
			cx,
			cy,
			cz,
			iso,
			key.pointCode,
			vertices.back(),
			colors.back()
		);
		/// insert vertex ID into map and also return it
		pointToIndex[key] = newVertexId;
		return newVertexId;
	}
}

///------------------------------------------------------------------------------

int dualmc::getDualPointCode(
	const int32_t cx,
	const int32_t cy,
	const int32_t cz,
	const uint8_t iso,
	const DMCEdgeCode edge
) const {
	int cubeCode = getCellCode(cx, cy, cz, iso);

	for (int i = 0; i < 4; ++i)
		if (dualPointsList[cubeCode][i] & edge) {
			return dualPointsList[cubeCode][i];
		}
	return 0;
}

///------------------------------------------------------------------------------

int dualmc::getCellCode(
	const int32_t cx, 
	const int32_t cy,
	const int32_t cz, 
	const uint8_t iso
) const {
	/// determine for each cube corner if it is outside or inside
	int code = 0;
	if (data[gA(cx, cy, cz)] >= iso)
		// Code += b
		code |= 1;
	if (data[gA(cx + 1, cy, cz)] >= iso)
		code |= 2;
	if (data[gA(cx, cy + 1, cz)] >= iso)
		code |= 4;
	if (data[gA(cx + 1, cy + 1, cz)] >= iso)
		code |= 8;
	if (data[gA(cx, cy, cz + 1)] >= iso)
		code |= 16;
	if (data[gA(cx + 1, cy, cz + 1)] >= iso)
		code |= 32;
	if (data[gA(cx, cy + 1, cz + 1)] >= iso)
		code |= 64;
	if (data[gA(cx + 1, cy + 1, cz + 1)] >= iso)
		code |= 128;
	return code;
}

///------------------------------------------------------------------------------

void dualmc::calculateDualPoint(
	const int32_t cx,
	const int32_t cy,
	const int32_t cz,
	const uint8_t iso,
	const int pointCode, 
	Vertex & v,
	uint8_t & color
) const {
	/// initialize the point with lower voxel coordinates
	v.x = cx;
	v.y = cy;
	v.z = cz;

	// Get UV(obj) color
	color = data[gA(cx, cy, cz)];

	/// compute the dual point as the mean of the face vertices belonging to the
	/// original marching cubes face
	Vertex p;
	p.x = 0;
	p.y = 0;
	p.z = 0;
	int points = 0;

	/// sum edge intersection vertices using the point code
	if (pointCode & EDGE0) {
		p.x += ((float)iso	- (float)data[gA(cx, cy, cz)]) 
			/ ((float)data[gA(cx + 1, cy, cz)] - (float)data[gA(cx, cy, cz)]);
		points++;
	}

	if (pointCode & EDGE1) {
		p.x += 1.0f;
		p.z += ((float)iso - (float)data[gA(cx + 1, cy, cz)])
			/ ((float)data[gA(cx + 1, cy, cz + 1)] - (float)data[gA(cx + 1, cy, cz)]);
		points++;
	}

	if (pointCode & EDGE2) {
		p.x += ((float)iso - (float)data[gA(cx, cy, cz + 1)])
			/ ((float)data[gA(cx + 1, cy, cz + 1)] - (float)data[gA(cx, cy, cz + 1)]);
		p.z += 1.0f;
		points++;
	}

	if (pointCode & EDGE3) {
		p.z += ((float)iso - (float)data[gA(cx, cy, cz)])
			/ ((float)data[gA(cx, cy, cz + 1)] - (float)data[gA(cx, cy, cz)]);
		points++;
	}

	if (pointCode & EDGE4) {
		p.x += ((float)iso - (float)data[gA(cx, cy + 1, cz)]) 
			/ ((float)data[gA(cx + 1, cy + 1, cz)] - (float)data[gA(cx, cy + 1, cz)]);
		p.y += 1.0f;
		points++;
	}

	if (pointCode & EDGE5) {
		p.x += 1.0f;
		p.z += ((float)iso - (float)data[gA(cx + 1, cy + 1, cz)])
			/ ((float)data[gA(cx + 1, cy + 1, cz + 1)] - (float)data[gA(cx + 1, cy + 1, cz)]);
		p.y += 1.0f;
		points++;
	}

	if (pointCode & EDGE6) {
		p.x += ((float)iso - (float)data[gA(cx, cy + 1, cz + 1)]) 
			/ ((float)data[gA(cx + 1, cy + 1, cz + 1)] - (float)data[gA(cx, cy + 1, cz + 1)]);
		p.z += 1.0f;
		p.y += 1.0f;
		points++;
	}

	if (pointCode & EDGE7) {
		p.z += ((float)iso - (float)data[gA(cx, cy + 1, cz)])
			/ ((float)data[gA(cx, cy + 1, cz + 1)] - (float)data[gA(cx, cy + 1, cz)]);
		p.y += 1.0f;
		points++;
	}

	if (pointCode & EDGE8) {
		p.y += ((float)iso - (float)data[gA(cx, cy, cz)]) 
			/ ((float)data[gA(cx, cy + 1, cz)] - (float)data[gA(cx, cy, cz)]);
		points++;
	}

	if (pointCode & EDGE9) {
		p.x += 1.0f;
		p.y += ((float)iso - (float)data[gA(cx + 1, cy, cz)])
			/ ((float)data[gA(cx + 1, cy + 1, cz)] - (float)data[gA(cx + 1, cy, cz)]);
		points++;
	}

	if (pointCode & EDGE10) {
		p.x += 1.0f;
		p.y += ((float)iso - (float)data[gA(cx + 1, cy, cz + 1)])
			/ ((float)data[gA(cx + 1, cy + 1, cz + 1)] - (float)data[gA(cx + 1, cy, cz + 1)]);
		p.z += 1.0f;
		points++;
	}

	if (pointCode & EDGE11) {
		p.z += 1.0f;
		p.y += ((float)iso - (float)data[gA(cx, cy, cz + 1)])
			/ ((float)data[gA(cx, cy + 1, cz + 1)] - (float)data[gA(cx, cy, cz + 1)]);
		points++;
	}

	/// divide by number of accumulated points
	float invPoints = 1.0f / (float)points;
	p.x *= invPoints;
	p.y *= invPoints;
	p.z *= invPoints;

	/// offset point by voxel coordinates
	v.x += p.x;
	v.y += p.y;
	v.z += p.z;
}
