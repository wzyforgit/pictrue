#include"picture.h"
#include"ansiLib.h"

/*打开24位BMP图片*/
static dm24* getDataMap_24bit(FILE* bmpFile, picHead *h)
{
	GETSIZE();
	dm24 *dataMap;
	/*申请存储图像数据的内存*/
	dataMap = (dm24*)malloc(sizeof(dm24)* height * width);
	if (dataMap == NULL)
	{
		printf("调色板内存申请失败!\n");
		system("pause");
		exit(1);
	}

	/*读取图像数据*/
	if ((width*h->infoHeader.biBitCount / 8) % 4 == 0)
	{
		fread(dataMap, sizeof(dm24), height * width, bmpFile);
	}

	else
	{
		int otherByte = 4 - ((width*h->infoHeader.biBitCount / 8) % 4);
		int x, y;
		BYTE* zero = (BYTE*)malloc(otherByte);
		if (zero == NULL)
		{
			printf("存零内存(读取)申请失败!\n");
			system("pause");
			exit(1);
		}
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				fread(&dataMap[COORDINATE], sizeof(dm24), 1, bmpFile);
			}
			fread(zero, sizeof(BYTE), otherByte, bmpFile);
		}
		free(zero);
	}

	return dataMap;
}

/*打开8位BMP图片*/
static dm8* getDataMap_8bit(FILE* bmpFile, picHead *h)
{
	RGB_QUAD_8bit rgbQuad_8bit[256];
	fread(rgbQuad_8bit, sizeof(RGB_QUAD_8bit), 256, bmpFile);/*读取调色板*/

	GETSIZE();
	dm8 *dataMap;
	/*申请存储图像数据的内存*/
	dataMap = (dm8*)malloc(sizeof(dm8)* height * width);
	if (dataMap == NULL)
	{
		printf("调色板内存申请失败!\n");
		system("pause");
		exit(1);
	}

	/*读取图像数据*/
	if ((width*h->infoHeader.biBitCount / 8) % 4 == 0)
	{
		fread(dataMap, sizeof(dm8), height * width, bmpFile);
	}

	else
	{
		int otherByte = 4 - ((width*h->infoHeader.biBitCount / 8) % 4);
		int x, y;
		BYTE* zero = (BYTE*)malloc(otherByte);
		if (zero == NULL)
		{
			printf("存零内存(读取)申请失败!\n");
			system("pause");
			exit(1);
		}
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				fread(&dataMap[COORDINATE], sizeof(dm8), 1, bmpFile);
			}
			fread(zero, sizeof(BYTE), otherByte, bmpFile);
		}
		free(zero);
	}

	return dataMap;
}

/*24位BMP图片存储*/
static void savePicture_24bit(FILE* bmpFile, picHead *h, dm24 *dataMap)
{
	GETSIZE();

	/*写入文件头*/
	h->fileHeader.bfType = 0x4d42;
	h->fileHeader.bfSize = 
		sizeof(picHead)+width * height * sizeof(dm24);
	h->fileHeader.bfReserved1 = 0;
	h->fileHeader.bfReserved2 = 0;
	h->fileHeader.bfOffBits = sizeof(BIT_MAP_FILE_HEADER)+sizeof(BIT_MAP_INFO_HEADER);

	/*写入信息头*/
	h->infoHeader.biSize = sizeof(BIT_MAP_INFO_HEADER);
	h->infoHeader.biPlanes = 1;
	h->infoHeader.biHeight = height;
	h->infoHeader.biWidth = width;
	h->infoHeader.biBitCount = 24;
	h->infoHeader.biClrImportant = 0;
	h->infoHeader.biClrUsed = 0;
	h->infoHeader.biSizeImage = width * height * sizeof(dm24);
	h->infoHeader.biCompression = 0;
	h->infoHeader.biXPelsPerMeter = 0;
	h->infoHeader.biYPelsPerMeter = 0;

	/*写入处理后的图像信息*/
	fwrite(h, sizeof(picHead), 1, bmpFile);

	/*存储像素数据*/
	if ((width*h->infoHeader.biBitCount / 8) % 4 == 0)
	{
		fwrite(dataMap, sizeof(dm24), height * width, bmpFile);
	}
	else
	{
		int otherByte = 4 - ((width*h->infoHeader.biBitCount / 8) % 4);
		int x, y;
		BYTE* zero = (BYTE*)malloc(otherByte);
		if (zero == NULL)
		{
			printf("存零内存(存储)申请失败!\n");
			system("pause");
			exit(1);
		}
		for (int count = 0; count < otherByte; count++)
		{
			zero[count] = 0;
		}
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				fwrite(&dataMap[COORDINATE], sizeof(dm24), 1, bmpFile);
			}
			fwrite(zero, sizeof(BYTE), otherByte, bmpFile);
		}
		free(zero);
	}
}

/*8位BMP图片存储*/
static void savePicture_8bit(FILE* bmpFile, picHead *h, dm8 *dataMap)
{
	GETSIZE();
	
	/*写入文件头*/
	h->fileHeader.bfType = 0x4d42;
	h->fileHeader.bfSize =
		sizeof(picHead)+256 * sizeof(RGB_QUAD_8bit)+width * height*sizeof(dm8);
	h->fileHeader.bfReserved1 = 0;
	h->fileHeader.bfReserved2 = 0;
	h->fileHeader.bfOffBits = sizeof(picHead)+256 * sizeof(RGB_QUAD_8bit);

	/*写入信息头*/
	h->infoHeader.biSize = sizeof(BIT_MAP_INFO_HEADER);
	h->infoHeader.biPlanes = 1;
	h->infoHeader.biHeight = height;
	h->infoHeader.biWidth = width;
	h->infoHeader.biBitCount = 8;
	h->infoHeader.biClrImportant = 0;
	h->infoHeader.biClrUsed = 0;
	h->infoHeader.biSizeImage = width * height*sizeof(dm8);
	h->infoHeader.biCompression = 0;
	h->infoHeader.biXPelsPerMeter = 0;
	h->infoHeader.biYPelsPerMeter = 0;

	/*写入处理后的图像信息*/
	fwrite(h, sizeof(picHead), 1, bmpFile);//写入图片信息
	RGB_QUAD_8bit rgbQuad_8bit[256];
	for (int count = 0; count < 256; count++)
	{
		rgbQuad_8bit[count].blue = rgbQuad_8bit[count].green = rgbQuad_8bit[count].red = count;
		rgbQuad_8bit[count].rgbReserved = 0;
	}
	fwrite(rgbQuad_8bit, sizeof(RGB_QUAD_8bit), 256, bmpFile);//写入调色板

	/*存储像素数据*/
	if ((width*h->infoHeader.biBitCount / 8) % 4 == 0)
	{
		fwrite(dataMap, sizeof(dm8), height * width, bmpFile);
	}
	else
	{
		int otherByte = 4 - ((width*h->infoHeader.biBitCount / 8) % 4);
		int x, y;
		BYTE* zero = (BYTE*)malloc(sizeof(BYTE)* otherByte);
		if (zero == NULL)
		{
			printf("存零内存(存储)申请失败!\n");
			system("pause");
			exit(1);
		}
		for (int count = 0; count < otherByte; count++)
		{
			zero[count] = 0;
		}
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				fwrite(&dataMap[COORDINATE], sizeof(dm8), 1, bmpFile);
			}
			fwrite(zero, sizeof(BYTE), otherByte, bmpFile);
		}
		free(zero);
	}

	fclose(bmpFile);
}

/*通用打开函数*/
void* getDataMap(char* fileName, picHead *h)
{
	FILE* bmpFile;
	void* dataMap = NULL;
	/*打开并读取文件头和信息头*/
	if ((bmpFile = fopen(fileName, "rb")) == NULL)
	{
		printf("文件打开失败!\n");
		system("pause");
		exit(1);
	}
	fread(h, sizeof(picHead), 1, bmpFile);

	if (h->fileHeader.bfType != 0x4d42)/*文件类型校验*/
	{
		printf("这不是BMP图片!\n");
		fclose(bmpFile);
		system("pause");
		exit(0);
	}

	switch (h->infoHeader.biBitCount)
	{
	case 24: dataMap = getDataMap_24bit(bmpFile, h); break;
	case 8:dataMap = getDataMap_8bit(bmpFile, h); break;
	default:printf("暂不支持打开这种格式的图片!\n"); exit(0);
	}
	fclose(bmpFile);
	return dataMap;
}

/*通用关闭函数*/
void savePicture(char* fileName, picHead *h, void *dataMap)
{
	if (dataMap == NULL)
	{
		printf("未传入像素数据!\n");
		system("pause");
		exit(0);
	}
	FILE* bmpFile;
	/*新建图像文件*/
	if ((bmpFile = fopen(fileName, "wb")) == NULL)
	{
		printf("文件创建失败!\n");
		system("pause");
		exit(1);
	}
	switch (h->infoHeader.biBitCount)
	{
	case 24: savePicture_24bit(bmpFile, h, dataMap); break;
	case 8:  savePicture_8bit(bmpFile, h, dataMap); break;
	default:printf("图片数据错误!\n"); exit(0);
	}
	fclose(bmpFile);
}