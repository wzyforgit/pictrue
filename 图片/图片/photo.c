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

/*定义位图文件头*/
typedef struct
{
	WORD bfType;//文件类型,必须为"BM",ASCII值为0x4d42
	DWORD bfSize;//文件的大小
	WORD bfReserved1;//为0
	WORD bfReserved2;//为0
	DWORD bfOffBits;//存储的像素阵列相对于文件头的偏移量(文件开头到调色区距离)
} BIT_MAP_FILE_HEADER;

/*定义信息头*/
typedef struct
{
	DWORD biSize;//信息头大小
	DWORD biWidth;//位图宽度(像素)
	DWORD biHeight;//位图高度(像素)
	WORD biPlanes;//必须为1
	WORD biBitCount;//每个像素的位数,即二进制数位数24为每一色8位(1,4,8,16,24,32)
	DWORD biCompression;//压缩方式(0或BI_RGB(未压缩))
	DWORD biSizeImage;//图像大小(字节,不包括信息头),仅用于压缩位图
	DWORD biXPelsPerMeter;//以目标设备每米的像素数来说明位图的水平分辨率
	DWORD biYPelsPerMeter; //以目标设备每米的像素数来说明位图的垂直分辨率
	DWORD biClrUsed;//颜色表的颜色数,若为0则位图使用由biBitCount指定的最大颜色数
	DWORD biClrImportant;//重要颜色的数目，若该值为0则所有颜色都重要
} BIT_MAP_INFO_HEADER;

/*调色板*/
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
	long width = 640;  //设置图片宽度
	long height = 240; //设置图片高度
	long date_size = width*height;
	BIT_MAP_FILE_HEADER bmp_map_file;
	BIT_MAP_INFO_HEADER bmp_map_info;

	if ((bmp_file = fopen("cubs.bmp", "wb+")) == NULL)
	{
		printf("Error!\n");
		system("pause");
		exit(1);
	}

	/*写入图位文件头*/
	bmp_map_file.bfType = 0x4d42;
	bmp_map_file.bfSize =\
		sizeof(BIT_MAP_FILE_HEADER) + sizeof(BIT_MAP_INFO_HEADER) + width * height * sizeof(RGB_QUAD);
	bmp_map_file.bfReserved1 = 0;
	bmp_map_file.bfReserved2 = 0;
	bmp_map_file.bfOffBits = sizeof(BIT_MAP_FILE_HEADER) + sizeof(BIT_MAP_INFO_HEADER);
	fwrite(&bmp_map_file, sizeof(BIT_MAP_FILE_HEADER), 1, bmp_file);

	/*写入信息头*/
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

	/*给图像分配内存*/
	if ((bmp_data = (RGB_QUAD*)malloc(width * height * sizeof(RGB_QUAD))) == NULL)
	{
		printf("申请内存失败！\n");
		exit(0);
	}
	/*构建RGB信息*/
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

	printf("BMP图像生成成功！\n\n\n");
	fclose(bmp_file);
	free(bmp_data);
	bmp_data = NULL;
	system("start cubs.bmp");
	return 0;
}