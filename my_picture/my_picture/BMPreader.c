#include"picture.h"
#include"ansiLib.h"

/*��24λBMPͼƬ*/
static dm24* getDataMap_24bit(FILE* bmpFile, picHead *h)
{
	GETSIZE();
	dm24 *dataMap;
	/*����洢ͼ�����ݵ��ڴ�*/
	dataMap = (dm24*)malloc(sizeof(dm24)* height * width);
	if (dataMap == NULL)
	{
		printf("��ɫ���ڴ�����ʧ��!\n");
		system("pause");
		exit(1);
	}

	/*��ȡͼ������*/
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
			printf("�����ڴ�(��ȡ)����ʧ��!\n");
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

/*��8λBMPͼƬ*/
static dm8* getDataMap_8bit(FILE* bmpFile, picHead *h)
{
	RGB_QUAD_8bit rgbQuad_8bit[256];
	fread(rgbQuad_8bit, sizeof(RGB_QUAD_8bit), 256, bmpFile);/*��ȡ��ɫ��*/

	GETSIZE();
	dm8 *dataMap;
	/*����洢ͼ�����ݵ��ڴ�*/
	dataMap = (dm8*)malloc(sizeof(dm8)* height * width);
	if (dataMap == NULL)
	{
		printf("��ɫ���ڴ�����ʧ��!\n");
		system("pause");
		exit(1);
	}

	/*��ȡͼ������*/
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
			printf("�����ڴ�(��ȡ)����ʧ��!\n");
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

/*24λBMPͼƬ�洢*/
static void savePicture_24bit(FILE* bmpFile, picHead *h, dm24 *dataMap)
{
	GETSIZE();

	/*д���ļ�ͷ*/
	h->fileHeader.bfType = 0x4d42;
	h->fileHeader.bfSize = 
		sizeof(picHead)+width * height * sizeof(dm24);
	h->fileHeader.bfReserved1 = 0;
	h->fileHeader.bfReserved2 = 0;
	h->fileHeader.bfOffBits = sizeof(BIT_MAP_FILE_HEADER)+sizeof(BIT_MAP_INFO_HEADER);

	/*д����Ϣͷ*/
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

	/*д�봦����ͼ����Ϣ*/
	fwrite(h, sizeof(picHead), 1, bmpFile);

	/*�洢��������*/
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
			printf("�����ڴ�(�洢)����ʧ��!\n");
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

/*8λBMPͼƬ�洢*/
static void savePicture_8bit(FILE* bmpFile, picHead *h, dm8 *dataMap)
{
	GETSIZE();
	
	/*д���ļ�ͷ*/
	h->fileHeader.bfType = 0x4d42;
	h->fileHeader.bfSize =
		sizeof(picHead)+256 * sizeof(RGB_QUAD_8bit)+width * height*sizeof(dm8);
	h->fileHeader.bfReserved1 = 0;
	h->fileHeader.bfReserved2 = 0;
	h->fileHeader.bfOffBits = sizeof(picHead)+256 * sizeof(RGB_QUAD_8bit);

	/*д����Ϣͷ*/
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

	/*д�봦����ͼ����Ϣ*/
	fwrite(h, sizeof(picHead), 1, bmpFile);//д��ͼƬ��Ϣ
	RGB_QUAD_8bit rgbQuad_8bit[256];
	for (int count = 0; count < 256; count++)
	{
		rgbQuad_8bit[count].blue = rgbQuad_8bit[count].green = rgbQuad_8bit[count].red = count;
		rgbQuad_8bit[count].rgbReserved = 0;
	}
	fwrite(rgbQuad_8bit, sizeof(RGB_QUAD_8bit), 256, bmpFile);//д���ɫ��

	/*�洢��������*/
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
			printf("�����ڴ�(�洢)����ʧ��!\n");
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

/*ͨ�ô򿪺���*/
void* getDataMap(char* fileName, picHead *h)
{
	FILE* bmpFile;
	void* dataMap = NULL;
	/*�򿪲���ȡ�ļ�ͷ����Ϣͷ*/
	if ((bmpFile = fopen(fileName, "rb")) == NULL)
	{
		printf("�ļ���ʧ��!\n");
		system("pause");
		exit(1);
	}
	fread(h, sizeof(picHead), 1, bmpFile);

	if (h->fileHeader.bfType != 0x4d42)/*�ļ�����У��*/
	{
		printf("�ⲻ��BMPͼƬ!\n");
		fclose(bmpFile);
		system("pause");
		exit(0);
	}

	switch (h->infoHeader.biBitCount)
	{
	case 24: dataMap = getDataMap_24bit(bmpFile, h); break;
	case 8:dataMap = getDataMap_8bit(bmpFile, h); break;
	default:printf("�ݲ�֧�ִ����ָ�ʽ��ͼƬ!\n"); exit(0);
	}
	fclose(bmpFile);
	return dataMap;
}

/*ͨ�ùرպ���*/
void savePicture(char* fileName, picHead *h, void *dataMap)
{
	if (dataMap == NULL)
	{
		printf("δ������������!\n");
		system("pause");
		exit(0);
	}
	FILE* bmpFile;
	/*�½�ͼ���ļ�*/
	if ((bmpFile = fopen(fileName, "wb")) == NULL)
	{
		printf("�ļ�����ʧ��!\n");
		system("pause");
		exit(1);
	}
	switch (h->infoHeader.biBitCount)
	{
	case 24: savePicture_24bit(bmpFile, h, dataMap); break;
	case 8:  savePicture_8bit(bmpFile, h, dataMap); break;
	default:printf("ͼƬ���ݴ���!\n"); exit(0);
	}
	fclose(bmpFile);
}