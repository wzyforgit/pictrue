/*若为VS2013及以上版本请添加预定义宏 _CRT_SECURE_NO_WARNINGS */
#define _CRT_SECURE_NO_WARNINGS
#ifndef PICTURE_H
#define PICTURE_H
#include <pshpack2.h>
#include <poppack.h>

/*****************************************基础数据***************************************/

typedef unsigned       char	uint8;  /*  8 bits */
typedef unsigned short int	uint16; /* 16 bits */
typedef unsigned long  int	uint32; /* 32 bits */

typedef signed char	int8;   /*  8 bits */
typedef short  int	int16;  /* 16 bits */
typedef signed int	int32;  /* 32 bits */

typedef float  float32; /*  32 bits */
typedef double float64; /*  64 bits */

/************************************BMP图片打开与存储***********************************/

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
}BIT_MAP_FILE_HEADER;

/*定义信息头*/
typedef struct
{
	DWORD biSize;//信息头大小
	DWORD biWidth;//位图宽度(像素)
	DWORD biHeight;//位图高度(像素)
	WORD biPlanes;//必须为1
	WORD biBitCount;//每个像素的位数,即二进制数位数24为每一色8位(1,4,8,16,24,32)
	DWORD biCompression;//压缩方式(0或BI_RGB(未压缩))
	DWORD biSizeImage;//图像大小(字节),仅用于压缩位图
	DWORD biXPelsPerMeter;//以目标设备每米的像素数来说明位图的水平分辨率
	DWORD biYPelsPerMeter; //以目标设备每米的像素数来说明位图的垂直分辨率
	DWORD biClrUsed;//颜色表的颜色数,若为0则位图使用由biBitCount指定的最大颜色数
	DWORD biClrImportant;//重要颜色的数目，若该值为0则所有颜色都重要
}BIT_MAP_INFO_HEADER;

typedef struct
{
	BIT_MAP_FILE_HEADER fileHeader;
	BIT_MAP_INFO_HEADER infoHeader;
}picHead;

/*调色板*/
typedef struct
{
	BYTE blue;
	BYTE green;
	BYTE red;
} RGB_QUAD_24bit;

typedef struct
{
	BYTE blue;
	BYTE green;
	BYTE red;
	BYTE rgbReserved;
} RGB_QUAD_8bit;

#pragma pack(pop)

/******************************************宏命令*****************************************/

/*24位图片数据阵列*/
typedef RGB_QUAD_24bit dm24;

/*8位图片数据阵列*/
typedef BYTE dm8;

/*布尔化掩码*/
#define MASK_BOOL 0x01

/*四舍五入*/
#define rounding(num) ((int32)(num + 0.5))

/*小数提取*/
#define lnum(num) (num-(int32)num)

/*获取图片尺寸*/
#define GETSIZE() \
	int32 height = h->infoHeader.biHeight; \
	int32 width = h->infoHeader.biWidth

/*最大灰度级*/
#define ASH_LEVEL 256

/*坐标*/
#define COORDINATE y*width+x
#define ABOVE     (y+1)*width+x
#define BELOW     (y-1)*width+x
#define LEFT       y*width+(x-1)
#define RIGHT      y*width+(x+1)

/*当前像素灰度值*/
#define THIS_ASH_LEVEL dataMap[COORDINATE]

/*二值化*/
#define BE_BLACK dataMap[COORDINATE] = 0;
#define BE_WHITE dataMap[COORDINATE] = 255;

/******************************************图片打开与存储*****************************************/

/*打开24位BMP图片*/
extern dm24* getDataMap_24bit(char* fileName, picHead *h);

/*打开8位BMP图片*/
extern dm8* getDataMap_8bit(char* fileName, picHead *h);

/*24位BMP图片存储*/
extern void savePicture_24bit(char* fileName, picHead *h, dm24 *dataMap);

/*8位BMP图片存储*/
extern void savePicture_8bit(char* fileName, picHead *h, dm8 *dataMap);

/******************************************灰度图像处理*****************************************/

/*灰度化(加权平均法)*/
extern dm8* getAsh(picHead *h, dm24 *dataMap);

/*获取灰度图像直方图*/
extern uint32* getHistogram(picHead *h, dm8 *dataMap);

/*灰度平均值*/
extern uint32 getAshAverage(picHead *h, uint32 *histogram);

/*灰度标准差*/
extern float64 getAshSTDEV(picHead *h, uint32 average, uint32 *histogram);

/*区域增强*/
/*一般取E=4.0,K0=0.4,K1=0.02,K2=0.4,可以根据实际情况作进一步调整*/
extern dm8* areaUp(float64 E, float64 k0, float64 k1, float64 k2, picHead *h, dm8 *dataMap);

/*获取最大,最小灰度值*/
extern void getAshMaxMin(uint8 *ashMax, uint8 *ashMin, uint32 *histogram);

/*直方图均衡化*/
extern uint32* balanceHistogram(picHead *h, uint32 *histogram);

/*直方图规定化*/         /*仍然存在问题*/
/*extern uint32* matchHistogram(picHead *h, dm8 *source, dm8 *aim, uint32 *histogram, uint32 *goal);*/

/*对应计算*/
extern dm8* getresult_histogram(uint32 *histogram, picHead *h, dm8 *dataMap);

/*灰度反转*/
extern dm8* exchengeWhiteBlack(picHead *h, dm8 *dataMap);

/*伽玛校正*/
extern dm8* gammaAdjust(float64 gamma, picHead *h, dm8 *dataMap);

/*对数校正*/
extern dm8* logAdjust(float64 coefficient, picHead *h, dm8 *dataMap);

/*对比拉伸*/
extern dm8* drawContrast(picHead *h, dm8* dataMap);

/*灰度切割*/
extern dm8* cutAsh(uint8 gray1, uint8 gray2, picHead *h, dm8 *dataMap);

/*位图切割*/
extern dm8* cutBitMap(uint8 level, picHead *h, dm8 *dataMap);

/*缩小两倍(最邻域删除)*/
extern dm8* reduceTwice(picHead *h, dm8 *dataMap);

/*放大两倍(最领域内插)*/
extern dm8* magnifyTwice(picHead *h, dm8 *dataMap);

/*双线性内插值(两倍)*/
extern dm8* magnifyTwice_linear(picHead *h, dm8 *dataMap);

/*高频提升滤波器(A=1时还原为标准拉普拉斯锐化)*/
extern dm8* laplace(float64 A, picHead *h, dm8 *dataMap);

/*Sobel算子(梯度变换,用于边缘增强)*/
extern dm8* sobel(picHead *h, dm8 *dataMap);

/*平滑线性滤波器(加权平均)*/
extern dm8* ave(picHead *h, dm8 *dataMap);

/*寻找中值*/
extern uint8 getMedian(uint8 *data, int32 num);

/*中值滤波器*/
extern dm8* median(picHead *h, dm8 *dataMap);

#endif  /*picture_h*/