/*若为VS2013及以上版本请添加预定义宏 _CRT_SECURE_NO_WARNINGS */
#define _CRT_SECURE_NO_WARNINGS
#ifndef PICTURE_H
#define PICTURE_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pshpack2.h>
#include <poppack.h>

/******************************************宏命令*****************************************/

/*布尔化掩码*/
#define MASK_BOOL 0x01

/*四舍五入*/
#define rounding(num) ((int32)((num) + 0.5))

/*小数提取*/
#define lnum(num) ((num)-(int32)(num))

/*图像宽度与高度*/
#define WIDTH  bitMapInfoHeader.biWidth
#define HEIGHT bitMapInfoHeader.biHeight

/*最大灰度级*/
#define ASH_LEVEL 256

/*坐标*/
#define COORDINATE y*WIDTH+x
#define ABOVE     (y+1)*WIDTH+x
#define BELOW     (y-1)*WIDTH+x
#define LEFT       y*WIDTH+(x-1)
#define RIGHT      y*WIDTH+(x+1)

/*当前像素灰度值*/
#define THIS_ASH_LEVEL rgbQuad[COORDINATE]

/*二值化*/
#define BE_BLACK rgbQuad[COORDINATE] = 0;
#define BE_WHITE rgbQuad[COORDINATE] = 255;

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
 BIT_MAP_FILE_HEADER bitMapFileHeader;

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
 BIT_MAP_INFO_HEADER bitMapInfoHeader;

/*调色板*/
typedef struct
{
	BYTE blue;
	BYTE green;
	BYTE red;
} RGB_QUAD;

typedef struct
{
	BYTE blue;
	BYTE green;
	BYTE red;
	BYTE rgbReserved;
} RGB_QUAD_8bit;

#pragma pack(pop)

/*文件打开与读取*/
extern RGB_QUAD* getDataMap(char* fileName, RGB_QUAD *dataMap);

/*文件存储*/
extern void savePicture(char* fileName, RGB_QUAD *rgbQuad);
extern void savePicture_ash(char* fileName, BYTE *rgbQuad);//存储灰度图像为256色BMP图片

/*********************************基于灰度图像的操作***********************************/

/*灰度化(加权平均法)*/
extern BYTE* getAsh(RGB_QUAD *rgbQuad);

/*灰度反转*/
extern BYTE* exchengeWhiteBlack(BYTE *rgbQuad);

/*伽玛校正*/
extern BYTE* gammaAdjust(double gamma, BYTE *rgbQuad);

/*对数校正*/
extern BYTE* logAdjust(double coefficient, BYTE *rgbQuad);

/*对比拉伸*/
extern BYTE* drawContrast(BYTE ashMax, BYTE ashMin, BYTE* rgbQuad);

/*灰度切割*/
extern BYTE* cutAsh(BYTE gray1, BYTE gray2, BYTE *rgbQuad);

/*位图切割*/
extern BYTE* cutBitMap(BYTE level, BYTE *rgbQuad);

/*缩小两倍(隔行/列删除)*/
extern BYTE* reduceTwice(BYTE *rgbQuad);

/*放大两倍(复制相邻行/列)*/
extern BYTE* magnifyTwice(BYTE *rgbQuad);

/*放大两倍(双线性内插值)*/
extern BYTE* magnifyTwice_linear(BYTE *rgbQuad);

/*高频提升滤波器(也叫反锐化掩蔽,其实就是锐化)(A=1时还原为标准拉普拉斯锐化)*/
extern BYTE* laplace(double A, BYTE *rgbQuad);

/*Sobel算子(梯度变换,用于边缘增强)*/
extern BYTE* sobel(BYTE *rgbQuad);

/*平滑线性滤波器(加权平均)*/
extern BYTE* ave(BYTE *rgbQuad);

/*中值滤波器*/
extern BYTE* median(BYTE *rgbQuad);

/*获取灰度图像直方图*/
extern void getHistogram(unsigned int *histogram, BYTE* rgbQuad);

/*获取最大,最小灰度值*/
extern void getAshMaxMin(BYTE *ashMax, BYTE *ashMin, unsigned int *histogram);

/*直方图均衡化*/
extern unsigned int* balanceHistogram(unsigned int *histogram);

/*直方图规定化*/
extern unsigned int* matchHistogram(unsigned int *histogram, unsigned int *goal);

/*区域增强*/
/*一般取E=4.0,K0=0.4,K1=0.02,K2=0.4,可以根据实际情况对参数作进一步调整*/
extern BYTE* areaUp(double E, double k0, double k1, double k2, unsigned int average, double STDEV, BYTE *rgbQuad);

/*对应计算*/
extern BYTE* getresult_histogram(unsigned int *histogram, BYTE *rgbQuad);

/*复制边框*/
extern BYTE* copyFrame(BYTE *rgbQuad);

/*灰度平均值*/
extern unsigned int getAshAverage(unsigned int *histogram);

/*灰度标准差*/
double getAshSTDEV(unsigned int average, unsigned int *histogram);

/*寻找中值*/
extern BYTE getMedian(BYTE *data, int num);

/*****************************************基础数据***************************************/

typedef unsigned       char	uint8;  /*  8 bits */
typedef unsigned short int	uint16; /* 16 bits */
typedef unsigned long  int	uint32; /* 32 bits */

typedef signed char	int8;   /*  8 bits */
typedef short  int	int16;  /* 16 bits */
typedef signed int	int32;  /* 32 bits */

typedef float  float32; /*  32 bits */
typedef double float64; /*  64 bits */

/********************************基于二值化图像的处理*************************************/

/*
	*	名称:二值化
	*	功能:二值化灰度图像
	*	参数:二值化阈值,灰度图像像素阵列
	*	返回:二值化后的像素阵列
*/
extern uint8* binaryzation(uint8 threshold, uint8 *rgbQuad);

/*
	*	名称:膨胀
	*	功能:膨胀黑色区域
	*	参数:二值化图像像素阵列
	*	返回:膨胀后的像素阵列
*/
extern uint8* expand(uint8 *rgbQuad);

/*
	*	名称:腐蚀
	*	功能:腐蚀黑色区域
	*	参数:二值化图像像素阵列
	*	返回:腐蚀后的像素阵列
*/
extern uint8* corrode(uint8 *rgbQuad);

/*
	*	名称:拉普拉斯锐化
	*	功能:利用拉普拉斯变换锐化二值化图像
	*	参数:二值化图像像素阵列
	*	返回:锐化后的像素阵列
*/
extern uint8* laplace_B(uint8 *rgbQuad);

/*
	*	名称:边界提取
	*	功能:提取二值化图像的黑色区域边界
	*	参数:二值化图像像素阵列
	*	返回:提取边界后的像素阵列
*/
extern uint8* getFrame(uint8 *rgbQuad);

/*
	*	名称:灰度反转
	*	功能:将二值化图像的黑色区域变白,白色区域变黑
	*	参数:二值化图像像素阵列
	*	返回:灰度反转后的像素阵列
*/
extern uint8* exchengeWhiteBlack_B(uint8 *rgbQuad);

#endif