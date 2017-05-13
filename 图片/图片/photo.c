#define _CRT_SECURE_NO_WARNINGS
extern const int photo[];
extern const int photo2[];
#define backGround 0xff
#define wordColor 0x00
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <pshpack2.h>
#include <poppack.h>

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

#pragma pack(push)
#pragma pack(1)

/*����λͼ�ļ�ͷ*/
typedef struct
{
	WORD bfType;//�ļ�����,����Ϊ"BM",ASCIIֵΪ0x4d42
	DWORD bfSize;//�ļ��Ĵ�С
	WORD bfReserved1;//Ϊ0
	WORD bfReserved2;//Ϊ0
	DWORD bfOffBits;//�洢����������������ļ�ͷ��ƫ����(�ļ���ͷ����ɫ������)
} BIT_MAP_FILE_HEADER;

/*������Ϣͷ*/
typedef struct
{
	DWORD biSize;//��Ϣͷ��С
	DWORD biWidth;//λͼ���(����)
	DWORD biHeight;//λͼ�߶�(����)
	WORD biPlanes;//����Ϊ1
	WORD biBitCount;//ÿ�����ص�λ��,����������λ��24Ϊÿһɫ8λ(1,4,8,16,24,32)
	DWORD biCompression;//ѹ����ʽ(0��BI_RGB(δѹ��))
	DWORD biSizeImage;//ͼ���С(�ֽ�,��������Ϣͷ),������ѹ��λͼ
	DWORD biXPelsPerMeter;//��Ŀ���豸ÿ�׵���������˵��λͼ��ˮƽ�ֱ���
	DWORD biYPelsPerMeter; //��Ŀ���豸ÿ�׵���������˵��λͼ�Ĵ�ֱ�ֱ���
	DWORD biClrUsed;//��ɫ�����ɫ��,��Ϊ0��λͼʹ����biBitCountָ���������ɫ��
	DWORD biClrImportant;//��Ҫ��ɫ����Ŀ������ֵΪ0��������ɫ����Ҫ
} BIT_MAP_INFO_HEADER;

/*��ɫ��*/
typedef struct
{
	BYTE blue;
	BYTE green;
	BYTE red;
} RGB_QUAD;

#pragma pack(pop)
RGB_QUAD *bmp_data = NULL;

void printWord(int s, int t,const unsigned char *word)
{
	int count;
	int x, y;
	for (y = t, count = 0; y > t - 16; y--, count++)
	{
		for (x = s; x < s + 8; x++)
		{
			if (word[count] & (1 << (s + 7 - x)))
			{
				bmp_data[y * 320 + x].red = wordColor;
				bmp_data[y * 320 + x].green = wordColor;
				bmp_data[y * 320 + x].blue = wordColor;
			}
			else
			{
				bmp_data[y * 320 + x].red = backGround;
				bmp_data[y * 320 + x].green = backGround;
				bmp_data[y * 320 + x].blue = backGround;
			}
		}
		for (count++; x < s + 16; x++)
		{
			if (word[count] & (1 << (s + 15 - x)))
			{
				bmp_data[y * 320 + x].red = wordColor;
				bmp_data[y * 320 + x].green = wordColor;
				bmp_data[y * 320 + x].blue = wordColor;
			}
			else
			{
				bmp_data[y * 320 + x].red = backGround;
				bmp_data[y * 320 + x].green = backGround;
				bmp_data[y * 320 + x].blue = backGround;
			}
		}
	}
}

void printAllWord(int s, int t, int num, const unsigned char *word)
{
	int x, y;
	int count;
	for (x = s, y = t, count = 0; x<s + num * 16; x += 16, count += 32)
	{
		printWord(x, y, &word[count]);
	}
}

int main()
{
//	RGB_QUAD *bmp_data = NULL;
	FILE *bmp_file;
	int i, j;
	long width = 640;  //����ͼƬ���
	long height = 240; //����ͼƬ�߶�
	long date_size = width*height;
	BIT_MAP_FILE_HEADER bmp_map_file;
	BIT_MAP_INFO_HEADER bmp_map_info;

	if ((bmp_file = fopen("cubs.bmp", "wb+")) == NULL)
	{
		printf("Error!\n");
		system("pause");
		exit(1);
	}

	/*д��ͼλ�ļ�ͷ*/
	bmp_map_file.bfType = 0x4d42;
	bmp_map_file.bfSize =\
		sizeof(BIT_MAP_FILE_HEADER) + sizeof(BIT_MAP_INFO_HEADER) + width * height * sizeof(RGB_QUAD);
	bmp_map_file.bfReserved1 = 0;
	bmp_map_file.bfReserved2 = 0;
	bmp_map_file.bfOffBits = sizeof(BIT_MAP_FILE_HEADER) + sizeof(BIT_MAP_INFO_HEADER);
	fwrite(&bmp_map_file, sizeof(BIT_MAP_FILE_HEADER), 1, bmp_file);

	/*д����Ϣͷ*/
	bmp_map_info.biSize = sizeof(BIT_MAP_INFO_HEADER);
	bmp_map_info.biPlanes = 1;
	bmp_map_info.biHeight = height;
	bmp_map_info.biWidth = width;
	bmp_map_info.biBitCount = 24;
	bmp_map_info.biClrImportant = 0;
	bmp_map_info.biClrUsed = 0;
	bmp_map_info.biSizeImage = width * height * sizeof(RGB_QUAD);
	bmp_map_info.biCompression = 0;
	bmp_map_info.biXPelsPerMeter = 0;
	bmp_map_info.biYPelsPerMeter = 0;
	fwrite(&bmp_map_info, sizeof(BIT_MAP_INFO_HEADER), 1, bmp_file);

	/*��ͼ������ڴ�*/
	if ((bmp_data = (RGB_QUAD*)malloc(width * height * sizeof(RGB_QUAD))) == NULL)
	{
		printf("�����ڴ�ʧ�ܣ�\n");
		exit(0);
	}
	/*����RGB��Ϣ*/
/*	for (j = 0; j < 240; j++)
	{
		for (i = 0; i < 320; i++)
		{
			bmp_data[j * 320 + i].red = backGround;
			bmp_data[j * 320 + i].green = backGround;
			bmp_data[j * 320 + i].blue = backGround;
		}
	}

	printAllWord(120, 120, 5, word);*/

	for (j = 0; j < 240; j++)
	{
		for (i = 0; i < 320; i++)
		{
			bmp_data[j * 640 + i].red = photo[j * 320 + i] >> 16;
			bmp_data[j * 640 + i].green = photo[j * 320 + i] >> 8 & 0xff;
			bmp_data[j * 640 + i].blue = photo[j * 320 + i] & 0xff;
		}
	}
	
	for (j = 0; j < 240; j++)
	{
		for (i = 320; i < 640; i++)
		{
			bmp_data[j * 640 + i].red = photo2[j * 320 + i - 320] >> 16;
			bmp_data[j * 640 + i].green = photo2[j * 320 + i - 320] >> 8 & 0xff;
			bmp_data[j * 640 + i].blue = photo2[j * 320 + i - 320] & 0xff;
		}
	}
	
	fwrite(bmp_data, sizeof(RGB_QUAD), date_size, bmp_file);

	printf("BMPͼ�����ɳɹ���\n\n\n");
	fclose(bmp_file);
	free(bmp_data);
	bmp_data = NULL;
	system("start cubs.bmp");
	return 0;
}