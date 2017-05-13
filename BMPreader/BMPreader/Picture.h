/*��ΪVS2013�����ϰ汾�����Ԥ����� _CRT_SECURE_NO_WARNINGS */
#define _CRT_SECURE_NO_WARNINGS
#ifndef PICTURE_H
#define PICTURE_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pshpack2.h>
#include <poppack.h>

/******************************************������*****************************************/

/*����������*/
#define MASK_BOOL 0x01

/*��������*/
#define rounding(num) ((int32)((num) + 0.5))

/*С����ȡ*/
#define lnum(num) ((num)-(int32)(num))

/*ͼ������߶�*/
#define WIDTH  bitMapInfoHeader.biWidth
#define HEIGHT bitMapInfoHeader.biHeight

/*���Ҷȼ�*/
#define ASH_LEVEL 256

/*����*/
#define COORDINATE y*WIDTH+x
#define ABOVE     (y+1)*WIDTH+x
#define BELOW     (y-1)*WIDTH+x
#define LEFT       y*WIDTH+(x-1)
#define RIGHT      y*WIDTH+(x+1)

/*��ǰ���ػҶ�ֵ*/
#define THIS_ASH_LEVEL rgbQuad[COORDINATE]

/*��ֵ��*/
#define BE_BLACK rgbQuad[COORDINATE] = 0;
#define BE_WHITE rgbQuad[COORDINATE] = 255;

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
 BIT_MAP_FILE_HEADER bitMapFileHeader;

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
 BIT_MAP_INFO_HEADER bitMapInfoHeader;

/*��ɫ��*/
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

/*�ļ������ȡ*/
extern RGB_QUAD* getDataMap(char* fileName, RGB_QUAD *dataMap);

/*�ļ��洢*/
extern void savePicture(char* fileName, RGB_QUAD *rgbQuad);
extern void savePicture_ash(char* fileName, BYTE *rgbQuad);//�洢�Ҷ�ͼ��Ϊ256ɫBMPͼƬ

/*********************************���ڻҶ�ͼ��Ĳ���***********************************/

/*�ҶȻ�(��Ȩƽ����)*/
extern BYTE* getAsh(RGB_QUAD *rgbQuad);

/*�Ҷȷ�ת*/
extern BYTE* exchengeWhiteBlack(BYTE *rgbQuad);

/*٤��У��*/
extern BYTE* gammaAdjust(double gamma, BYTE *rgbQuad);

/*����У��*/
extern BYTE* logAdjust(double coefficient, BYTE *rgbQuad);

/*�Ա�����*/
extern BYTE* drawContrast(BYTE ashMax, BYTE ashMin, BYTE* rgbQuad);

/*�Ҷ��и�*/
extern BYTE* cutAsh(BYTE gray1, BYTE gray2, BYTE *rgbQuad);

/*λͼ�и�*/
extern BYTE* cutBitMap(BYTE level, BYTE *rgbQuad);

/*��С����(����/��ɾ��)*/
extern BYTE* reduceTwice(BYTE *rgbQuad);

/*�Ŵ�����(����������/��)*/
extern BYTE* magnifyTwice(BYTE *rgbQuad);

/*�Ŵ�����(˫�����ڲ�ֵ)*/
extern BYTE* magnifyTwice_linear(BYTE *rgbQuad);

/*��Ƶ�����˲���(Ҳ�з����ڱ�,��ʵ������)(A=1ʱ��ԭΪ��׼������˹��)*/
extern BYTE* laplace(double A, BYTE *rgbQuad);

/*Sobel����(�ݶȱ任,���ڱ�Ե��ǿ)*/
extern BYTE* sobel(BYTE *rgbQuad);

/*ƽ�������˲���(��Ȩƽ��)*/
extern BYTE* ave(BYTE *rgbQuad);

/*��ֵ�˲���*/
extern BYTE* median(BYTE *rgbQuad);

/*��ȡ�Ҷ�ͼ��ֱ��ͼ*/
extern void getHistogram(unsigned int *histogram, BYTE* rgbQuad);

/*��ȡ���,��С�Ҷ�ֵ*/
extern void getAshMaxMin(BYTE *ashMax, BYTE *ashMin, unsigned int *histogram);

/*ֱ��ͼ���⻯*/
extern unsigned int* balanceHistogram(unsigned int *histogram);

/*ֱ��ͼ�涨��*/
extern unsigned int* matchHistogram(unsigned int *histogram, unsigned int *goal);

/*������ǿ*/
/*һ��ȡE=4.0,K0=0.4,K1=0.02,K2=0.4,���Ը���ʵ������Բ�������һ������*/
extern BYTE* areaUp(double E, double k0, double k1, double k2, unsigned int average, double STDEV, BYTE *rgbQuad);

/*��Ӧ����*/
extern BYTE* getresult_histogram(unsigned int *histogram, BYTE *rgbQuad);

/*���Ʊ߿�*/
extern BYTE* copyFrame(BYTE *rgbQuad);

/*�Ҷ�ƽ��ֵ*/
extern unsigned int getAshAverage(unsigned int *histogram);

/*�Ҷȱ�׼��*/
double getAshSTDEV(unsigned int average, unsigned int *histogram);

/*Ѱ����ֵ*/
extern BYTE getMedian(BYTE *data, int num);

/*****************************************��������***************************************/

typedef unsigned       char	uint8;  /*  8 bits */
typedef unsigned short int	uint16; /* 16 bits */
typedef unsigned long  int	uint32; /* 32 bits */

typedef signed char	int8;   /*  8 bits */
typedef short  int	int16;  /* 16 bits */
typedef signed int	int32;  /* 32 bits */

typedef float  float32; /*  32 bits */
typedef double float64; /*  64 bits */

/********************************���ڶ�ֵ��ͼ��Ĵ���*************************************/

/*
	*	����:��ֵ��
	*	����:��ֵ���Ҷ�ͼ��
	*	����:��ֵ����ֵ,�Ҷ�ͼ����������
	*	����:��ֵ�������������
*/
extern uint8* binaryzation(uint8 threshold, uint8 *rgbQuad);

/*
	*	����:����
	*	����:���ͺ�ɫ����
	*	����:��ֵ��ͼ����������
	*	����:���ͺ����������
*/
extern uint8* expand(uint8 *rgbQuad);

/*
	*	����:��ʴ
	*	����:��ʴ��ɫ����
	*	����:��ֵ��ͼ����������
	*	����:��ʴ�����������
*/
extern uint8* corrode(uint8 *rgbQuad);

/*
	*	����:������˹��
	*	����:����������˹�任�񻯶�ֵ��ͼ��
	*	����:��ֵ��ͼ����������
	*	����:�񻯺����������
*/
extern uint8* laplace_B(uint8 *rgbQuad);

/*
	*	����:�߽���ȡ
	*	����:��ȡ��ֵ��ͼ��ĺ�ɫ����߽�
	*	����:��ֵ��ͼ����������
	*	����:��ȡ�߽�����������
*/
extern uint8* getFrame(uint8 *rgbQuad);

/*
	*	����:�Ҷȷ�ת
	*	����:����ֵ��ͼ��ĺ�ɫ������,��ɫ������
	*	����:��ֵ��ͼ����������
	*	����:�Ҷȷ�ת�����������
*/
extern uint8* exchengeWhiteBlack_B(uint8 *rgbQuad);

#endif