#pragma once

#ifndef CONVERTBMPAPI
#define CONVERTBMPAPI __declspec(dllimport)
#endif

struct DcmData{
	unsigned char * buffer;		//文件夹下所有图像像素数据
	unsigned short columns;		//像素列数
	unsigned short rows;		//像素行数
	int countImages;			//文件夹下图像个数
	size_t bufferSize;			//所有像素数据大小
	int RescaleIntercept;		//Rescale Intercept
	unsigned short RescaleSlope;			//Rescale Slope
};


//*dcmData: 传入结构体类型的地址，其中DcmData 成员 *buffer 需要手动释放；
//if success will return zero,else return less than zero; 
CONVERTBMPAPI int __stdcall converttobmp(const char * filename, DcmData *dcmData);
CONVERTBMPAPI char* __stdcall Decode(char* addr, int size);
CONVERTBMPAPI void __stdcall Release(char*addr);
