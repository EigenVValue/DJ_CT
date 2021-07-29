#ifndef GETIMAGEDATA_HPP
#define GETIMAGEDATA_HPP

void getImageData(
	const char* path,
	std::vector<uint8_t> &raw,
	unsigned int &dimX,
	unsigned int &dimY,
	unsigned int &dimZ,
	int &rescale_intercept,
	unsigned short &rescale_slope
);

void removeNoise(
	std::vector<uint8_t> &raw,
	unsigned int &dimX,
	unsigned int &dimY,
	unsigned int &dimZ,
	uint8_t threshold
);

#endif