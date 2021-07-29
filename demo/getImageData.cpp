#include <Windows.h>
#include <vector>

#include "getImageData.hpp"

// Inclued WANLI's file
#include "converttobmp.h"

void getImageData(
		const char* path, 
		std::vector<uint8_t> &raw,
		unsigned int &dimX,
		unsigned int &dimY,
		unsigned int &dimZ,
		int &rescale_intercept,
		unsigned short &rescale_slope
){
	// Struct see converttobmp.h file
	//struct DcmData {
	//	unsigned char * buffer;		//文件夹下所有图像像素数据
	//	unsigned short columns;		//像素列数
	//	unsigned short rows;		//像素行数
	//	int countImages;			//文件夹下图像个数
	//	size_t bufferSize;			//所有像素数据大小
	//	int RescaleIntercept;		//Rescale Intercept
	//	unsigned short RescaleSlope;			//Rescale Slope
	//};
	DcmData data;
	int bufferSize = converttobmp(
		path,
		&data
	);
	
	// get values
	dimX = data.columns;
	dimY = data.rows;
	dimZ = data.countImages;
	rescale_intercept = data.RescaleIntercept;
	rescale_slope = data.RescaleSlope;

	// char size == dim of x (height) * dim of y (width) * dim of z(total number of dcm files)
	raw.resize(data.rows*data.columns*data.countImages);

	for (int z = 0; z < dimZ; z++)
	{
		for (int y = 0; y < dimY; y++)
		{
			for (int x = 0; x < dimX; x++)
			{
				uint8_t tmp = data.buffer[
					(x + y * dimX + z * dimX * dimY) * 4
					+ sizeof(BITMAPFILEHEADER)*(z+1)
					+ sizeof(BITMAPINFOHEADER)*(z+1)
				];
				raw[x + y * dimX + z * dimX * dimY] = tmp;
			}
		}
	}

	delete data.buffer;
}

void removeNoise(
	std::vector<uint8_t> &raw,
	unsigned int &dimX,
	unsigned int &dimY,
	unsigned int &dimZ,
	uint8_t threshold
) {
	for (int z = 0; z < dimZ; z++)
	{
		for (int y = 0; y < dimY; y++)
		{
			for (int x = 0; x < dimX; x++)
			{
				// If current grayscale is larger than threshold
				// do search
				if (raw[x + y * dimX + z * dimX * dimY]
					>= threshold)
				{
					int zz = z;
					int same = 0;
					do
					{
						if (raw[x + y * dimX + zz * dimX * dimY]
								>= threshold)
						{
							same++;
							zz++;
						}
						else
						{
							break;
						}
					} while (zz < dimZ);

					// Fixed number 0.1 * size of Z
					if (same >= unsigned int(0.3*dimZ)) // 0.1 * dimZ
					{
						for (int i = 0; i < same; i++)
						{
							raw[x + y * dimX + (z+i) * dimX * dimY] = 0;
						}
					}
				} // IF END
			} // FOR X END
		} // FOR Y END
	} // FOR Z END
}