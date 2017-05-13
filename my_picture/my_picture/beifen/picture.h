/*��ΪVS2013�����ϰ汾�����Ԥ����� _CRT_SECURE_NO_WARNINGS */
#define _CRT_SECURE_NO_WARNINGS
#ifndef PICTURE_H
#define PICTURE_H
#include <pshpack2.h>
#include <poppack.h>

/*****************************************��������***************************************/

typedef unsigned       char	uint8;  /*  8 bits */
typedef unsigned short int	uint16; /* 16 bits */
typedef unsigned long  int	uint32; /* 32 bits */

typedef signed char	int8;   /*  8 bits */
typedef short  int	int16;  /* 16 bits */
typedef signed int	int32;  /* 32 bits */

typedef float  float32; /*  32 bits */
typedef double float64; /*  64 bits */

/************************************BMPͼƬ����洢***********************************/

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
}BIT_MAP_FILE_HEADER;

/*������Ϣͷ*/
typedef struct
{
	DWORD biSize;//��Ϣͷ��С
	DWORD biWidth;//λͼ���(����)
	DWORD biHeight;//λͼ�߶�(����)
	WORD biPlanes;//����Ϊ1
	WORD biBitCount;//ÿ�����ص�λ��,����������λ��24Ϊÿһɫ8λ(1,4,8,16,24,32)
	DWORD biCompression;//ѹ����ʽ(0��BI_RGB(δѹ��))
	DWORD biSizeImage;//ͼ���С(�ֽ�),������ѹ��λͼ
	DWORD biXPelsPerMeter;//��Ŀ���豸ÿ�׵���������˵��λͼ��ˮƽ�ֱ���
	DWORD biYPelsPerMeter; //��Ŀ���豸ÿ�׵���������˵��λͼ�Ĵ�ֱ�ֱ���
	DWORD biClrUsed;//��ɫ�����ɫ��,��Ϊ0��λͼʹ����biBitCountָ���������ɫ��
	DWORD biClrImportant;//��Ҫ��ɫ����Ŀ������ֵΪ0��������ɫ����Ҫ
}BIT_MAP_INFO_HEADER;

typedef struct
{
	BIT_MAP_FILE_HEADER fileHeader;
	BIT_MAP_INFO_HEADER infoHeader;
}picHead;

/*��ɫ��*/
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

/******************************************������*****************************************/

/*24λͼƬ��������*/
typedef RGB_QUAD_24bit dm24;

/*8λͼƬ��������*/
typedef BYTE dm8;

/*����������*/
#define MASK_BOOL 0x01

/*��������*/
#define rounding(num) ((int32)(num + 0.5))

/*С����ȡ*/
#define lnum(num) (num-(int32)num)

/*��ȡͼƬ�ߴ�*/
#define GETSIZE() \
	int32 height = h->infoHeader.biHeight; \
	int32 width = h->infoHeader.biWidth

/*���Ҷȼ�*/
#define ASH_LEVEL 256

/*����*/
#define COORDINATE y*width+x
#define ABOVE     (y+1)*width+x
#define BELOW     (y-1)*width+x
#define LEFT       y*width+(x-1)
#define RIGHT      y*width+(x+1)

/*��ǰ���ػҶ�ֵ*/
#define THIS_ASH_LEVEL dataMap[COORDINATE]

/*��ֵ��*/
#define BE_BLACK dataMap[COORDINATE] = 0;
#define BE_WHITE dataMap[COORDINATE] = 255;

/******************************************ͼƬ����洢*****************************************/

/*��24λBMPͼƬ*/
extern dm24* getDataMap_24bit(char* fileName, picHead *h);

/*��8λBMPͼƬ*/
extern dm8* getDataMap_8bit(char* fileName, picHead *h);

/*24λBMPͼƬ�洢*/
extern void savePicture_24bit(char* fileName, picHead *h, dm24 *dataMap);

/*8λBMPͼƬ�洢*/
extern void savePicture_8bit(char* fileName, picHead *h, dm8 *dataMap);

/******************************************�Ҷ�ͼ����*****************************************/

/*�ҶȻ�(��Ȩƽ����)*/
extern dm8* getAsh(picHead *h, dm24 *dataMap);

/*��ȡ�Ҷ�ͼ��ֱ��ͼ*/
extern uint32* getHistogram(picHead *h, dm8 *dataMap);

/*�Ҷ�ƽ��ֵ*/
extern uint32 getAshAverage(picHead *h, uint32 *histogram);

/*�Ҷȱ�׼��*/
extern float64 getAshSTDEV(picHead *h, uint32 average, uint32 *histogram);

/*������ǿ*/
/*һ��ȡE=4.0,K0=0.4,K1=0.02,K2=0.4,���Ը���ʵ���������һ������*/
extern dm8* areaUp(float64 E, float64 k0, float64 k1, float64 k2, picHead *h, dm8 *dataMap);

/*��ȡ���,��С�Ҷ�ֵ*/
extern void getAshMaxMin(uint8 *ashMax, uint8 *ashMin, uint32 *histogram);

/*ֱ��ͼ���⻯*/
extern uint32* balanceHistogram(picHead *h, uint32 *histogram);

/*ֱ��ͼ�涨��*/         /*��Ȼ��������*/
/*extern uint32* matchHistogram(picHead *h, dm8 *source, dm8 *aim, uint32 *histogram, uint32 *goal);*/

/*��Ӧ����*/
extern dm8* getresult_histogram(uint32 *histogram, picHead *h, dm8 *dataMap);

/*�Ҷȷ�ת*/
extern dm8* exchengeWhiteBlack(picHead *h, dm8 *dataMap);

/*٤��У��*/
extern dm8* gammaAdjust(float64 gamma, picHead *h, dm8 *dataMap);

/*����У��*/
extern dm8* logAdjust(float64 coefficient, picHead *h, dm8 *dataMap);

/*�Ա�����*/
extern dm8* drawContrast(picHead *h, dm8* dataMap);

/*�Ҷ��и�*/
extern dm8* cutAsh(uint8 gray1, uint8 gray2, picHead *h, dm8 *dataMap);

/*λͼ�и�*/
extern dm8* cutBitMap(uint8 level, picHead *h, dm8 *dataMap);

/*��С����(������ɾ��)*/
extern dm8* reduceTwice(picHead *h, dm8 *dataMap);

/*�Ŵ�����(�������ڲ�)*/
extern dm8* magnifyTwice(picHead *h, dm8 *dataMap);

/*˫�����ڲ�ֵ(����)*/
extern dm8* magnifyTwice_linear(picHead *h, dm8 *dataMap);

/*��Ƶ�����˲���(A=1ʱ��ԭΪ��׼������˹��)*/
extern dm8* laplace(float64 A, picHead *h, dm8 *dataMap);

/*Sobel����(�ݶȱ任,���ڱ�Ե��ǿ)*/
extern dm8* sobel(picHead *h, dm8 *dataMap);

/*ƽ�������˲���(��Ȩƽ��)*/
extern dm8* ave(picHead *h, dm8 *dataMap);

/*Ѱ����ֵ*/
extern uint8 getMedian(uint8 *data, int32 num);

/*��ֵ�˲���*/
extern dm8* median(picHead *h, dm8 *dataMap);

#endif  /*picture_h*/