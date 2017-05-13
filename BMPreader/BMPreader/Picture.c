#include"Picture.h"

/*�ļ������ȡ*/
RGB_QUAD* getDataMap(char* fileName, RGB_QUAD *dataMap)
{
	FILE* bmpFile;
	/*�򿪲���ȡ�ļ�ͷ����Ϣͷ*/
	if ((bmpFile = fopen(fileName, "rb")) == NULL)
	{
		printf("�ļ���ʧ��!\n");
		system("pause");
		exit(1);
	}
	fread(&bitMapFileHeader, sizeof(bitMapFileHeader), 1, bmpFile);
	if (bitMapFileHeader.bfType != 0x4d42)/*�ļ�����У��*/
	{
		printf("�ⲻ��BMPͼƬ!\n");
		fclose(bmpFile);
		system("pause");
		exit(0);
	}
	fread(&bitMapInfoHeader, sizeof(bitMapInfoHeader), 1, bmpFile);
	if (bitMapInfoHeader.biBitCount != 24)/*ͼƬ����У��*/
	{
		printf("�ⲻ��24λͼƬ!\n");
		fclose(bmpFile);
		system("pause");
		exit(0);
	}

	/*����洢ͼ�����ݵ��ڴ�*/
	dataMap = (RGB_QUAD*)malloc(sizeof(RGB_QUAD)* HEIGHT * WIDTH);
	if (dataMap == NULL)
	{
		printf("��ɫ���ڴ�����ʧ��!\n");
		system("pause");
		exit(1);
	}

	/*��ȡͼ������*/
	if ((WIDTH*bitMapInfoHeader.biBitCount / 8) % 4 == 0)
	{
		fread(dataMap, sizeof(RGB_QUAD), HEIGHT * WIDTH, bmpFile);
	}

	else
	{
		int otherByte = 4 - ((WIDTH*bitMapInfoHeader.biBitCount / 8) % 4);
		unsigned int x, y;
		BYTE* zero = (BYTE*)malloc(otherByte);
		if (zero == NULL)
		{
			printf("�����ڴ�(��ȡ)����ʧ��!\n");
			system("pause");
			exit(1);
		}
		for (y = 0; y < HEIGHT; y++)
		{
			for (x = 0; x < WIDTH; x++)
			{
				fread(&dataMap[COORDINATE], sizeof(RGB_QUAD), 1, bmpFile);
			}
			fread(zero, sizeof(BYTE), otherByte, bmpFile);
		}
		free(zero);
	}

	fclose(bmpFile);
	return dataMap;
}

/*�ļ��洢*/
void savePicture(char* fileName, RGB_QUAD *rgbQuad)
{
	if (rgbQuad == NULL)
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

	/*д�봦����ͼ����Ϣ*/
	fwrite(&bitMapFileHeader, sizeof(bitMapFileHeader), 1, bmpFile);
	fwrite(&bitMapInfoHeader, sizeof(bitMapInfoHeader), 1, bmpFile);

	/*�洢��������*/
	if ((WIDTH*bitMapInfoHeader.biBitCount / 8) % 4 == 0)
	{
		fwrite(rgbQuad, sizeof(RGB_QUAD), HEIGHT * WIDTH, bmpFile);
	}
	else
	{
		int otherByte = 4 - ((WIDTH*bitMapInfoHeader.biBitCount / 8) % 4);
		unsigned int x, y;
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
		for (y = 0; y < HEIGHT; y++)
		{
			for (x = 0; x < WIDTH; x++)
			{
				fwrite(&rgbQuad[COORDINATE], sizeof(RGB_QUAD), 1, bmpFile);
			}
			fwrite(zero, sizeof(BYTE), otherByte, bmpFile);
		}
		free(zero);
	}

	fclose(bmpFile);
}

/*�Ҷ�ͼ��洢(�洢Ϊ256ɫλͼ)*/
void savePicture_ash(char* fileName, BYTE *rgbQuad)
{
	if (rgbQuad == NULL)
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

	/*�����ļ�/��Ϣͷ,��ʼ����ɫ��*/
	bitMapFileHeader.bfOffBits = 54 + 1024;
	bitMapFileHeader.bfSize = bitMapFileHeader.bfOffBits + HEIGHT * WIDTH;
	bitMapInfoHeader.biBitCount = 8;
	bitMapInfoHeader.biSizeImage = HEIGHT * WIDTH;
	RGB_QUAD_8bit rgbQuad_8bit[256];
	for (int count = 0; count < 256; count++)
	{
		rgbQuad_8bit[count].blue = rgbQuad_8bit[count].green = rgbQuad_8bit[count].red = count;
		rgbQuad_8bit[count].rgbReserved = 0;
	}

	/*д�봦����ͼ����Ϣ*/
	fwrite(&bitMapFileHeader, sizeof(bitMapFileHeader), 1, bmpFile);//�ļ�ͷ
	fwrite(&bitMapInfoHeader, sizeof(bitMapInfoHeader), 1, bmpFile);//��Ϣͷ
	fwrite(rgbQuad_8bit, sizeof(RGB_QUAD_8bit), 256, bmpFile);//��ɫ��

	/*�洢��������*/
	if ((WIDTH*bitMapInfoHeader.biBitCount / 8) % 4 == 0)
	{
		fwrite(rgbQuad, sizeof(BYTE), HEIGHT * WIDTH, bmpFile);
	}
	else
	{
		int otherByte = 4 - ((WIDTH*bitMapInfoHeader.biBitCount / 8) % 4);
		unsigned int x, y;
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
		for (y = 0; y < HEIGHT; y++)
		{
			for (x = 0; x < WIDTH; x++)
			{
				fwrite(&rgbQuad[COORDINATE], sizeof(BYTE), 1, bmpFile);
			}
			fwrite(zero, sizeof(BYTE), otherByte, bmpFile);
		}
		free(zero);
	}

	fclose(bmpFile);
}

/*���Ʊ߿�*/
BYTE* copyFrame(BYTE *rgbQuad)
{	
	/*Ƶ��malloc���ܵ����ڴ���Ƭ���������,��ʱ�޷����*/
	BYTE *newDataMap = (BYTE*)malloc(sizeof(BYTE)* HEIGHT * WIDTH);
	if (newDataMap == NULL)
	{
		printf("���Ʊ߿��ڴ����ʧ��\n");
		system("pause");
		exit(1);
	}
	/*���Ʊ߿�*/
	unsigned int x, y;
	y = 0;
	for (x = 0; x < WIDTH; x++)
	{
//		newDataMap[COORDINATE] = THIS_ASH_LEVEL;
		newDataMap[COORDINATE] = 0;
	}
	for (y = 1; y < HEIGHT - 1; y++)
	{
		x = 0;
//		newDataMap[COORDINATE] = THIS_ASH_LEVEL;
		newDataMap[COORDINATE] = 0;
		x = x + WIDTH - 1;
//		newDataMap[COORDINATE] = THIS_ASH_LEVEL;
		newDataMap[COORDINATE] = 0;
	}
	for (x = 0; x < WIDTH; x++)
	{
//		newDataMap[COORDINATE] = THIS_ASH_LEVEL;
		newDataMap[COORDINATE] = 0;
	}
	return newDataMap;
}

/*�ҶȻ�(��Ȩƽ����)*/
#define ASH_VALUE \
(rgbQuad[COORDINATE].blue * 11 + rgbQuad[COORDINATE].green * 59 + rgbQuad[COORDINATE].red * 30) / 100
BYTE* getAsh(RGB_QUAD *rgbQuad)
{
	unsigned int x, y;
	BYTE* ashMap;
	ashMap = (BYTE*)malloc(sizeof(BYTE)*WIDTH*HEIGHT);
	if (ashMap == NULL)
	{
		printf("�Ҷ�ͼ��ɫ���ڴ�����ʧ��!\n");
		system("pause");
		exit(1);
	}
	for (y = 0; y < HEIGHT; y++)
	{
		for (x = 0; x < WIDTH; x++)
		{
			ashMap[COORDINATE] = ASH_VALUE;
		}
	}
	return ashMap;
}
#undef ASH_VALUE

/*��ȡ�Ҷ�ͼ��ֱ��ͼ*/
#define GOAL_HISTOGRAM_DATA histogram[rgbQuad[COORDINATE]]
void getHistogram(unsigned int *histogram, BYTE *rgbQuad)
{
	unsigned int x, y;
	for (y = 0; y < HEIGHT; y++)
	{
		for (x = 0; x < WIDTH; x++)
		{
			GOAL_HISTOGRAM_DATA++;
		}
	}
}
#undef GOAL_HISTOGRAM_DATA

/*�Ҷ�ƽ��ֵ*/
unsigned int getAshAverage(unsigned int *histogram)
{
	unsigned int sum = 0;
	unsigned int count;
	for (count = 0; count < 256; count++)
	{
		sum += count * histogram[count];
	}
	return sum / (WIDTH*HEIGHT);
}

/*�Ҷȱ�׼��*/
double getAshSTDEV(unsigned int average, unsigned int *histogram)
{
	double sum = 0;
	double EVmap[ASH_LEVEL];
	for (int count = 0; count < ASH_LEVEL; count++)
	{
		EVmap[count] = pow(count - (int)average, 2);
	}

	for (int count = 0; count < ASH_LEVEL; count++)
	{
		sum = sum + (histogram[count] * EVmap[count]);
	}

	return (sqrt(sum / (HEIGHT*WIDTH)));
}

/*��ȡ���,��С�Ҷ�ֵ*/
void getAshMaxMin(BYTE *ashMax, BYTE *ashMin, unsigned int *histogram)
{
	for (int count = 0; count < ASH_LEVEL; count++)
	{
		if (histogram[count] != 0)
		{
			*ashMin = count;
			break;
		}
	}
	for (int count = ASH_LEVEL - 1; count > *ashMin; count--)
	{
		if (histogram[count] != 0)
		{
			*ashMax = count;
			break;
		}
	}
}

/*ֱ��ͼ���⻯*/
unsigned int* balanceHistogram(unsigned int *histogram)
{
	unsigned int x, y;
	double pHistoGram[ASH_LEVEL];//���ָ���,��һ��ֱ��ͼ
	double cHistoGram[ASH_LEVEL];//�ۻ���һ��ֱ��ͼ(���⻯���)
	for (int count = 0; count < ASH_LEVEL; count++)//��һ��
	{
		pHistoGram[count] = (double)histogram[count] / (HEIGHT*WIDTH);
	}
	for (y = 0; y < ASH_LEVEL; y++)//���⻯
	{
		cHistoGram[y] = 0;
		for (x = 0; x <= y; x++)
		{
			cHistoGram[y] += pHistoGram[x];
		}
	}
	for (int count = 0; count < ASH_LEVEL; count++)//�������ս��
	{
		histogram[count] = rounding(cHistoGram[count] * 255);
	}

	return histogram;
}

/*ֱ��ͼ�涨��*/
unsigned int* matchHistogram(unsigned int *histogram, unsigned int *goal)
{
	histogram = balanceHistogram(histogram);
	goal = balanceHistogram(goal);
	unsigned int x, y;
	unsigned int difference, temp, temps;
	for (y = 0; y < ASH_LEVEL; y++)
	{
		for (x = 0, temp = ASH_LEVEL; x < ASH_LEVEL; x++)
		{
			if ((difference = abs(histogram[y] - goal[x]) == 0))
			{
				histogram[y] = x;
				break;
			}
			if (difference < temp)
			{
				temp = difference;
				temps = x;
			}
		}
		if (x == ASH_LEVEL)
			histogram[y] = temps;
	}
	return histogram;
}

/*������ǿ*/
/*һ��ȡE=4.0,K0=0.4,K1=0.02,K2=0.4,���Ը���ʵ���������һ������*/
BYTE* areaUp(double E, double k0, double k1, double k2, unsigned int average, double STDEV, BYTE *rgbQuad)
{	
	BYTE resultMap[ASH_LEVEL];
	for (int count = 0; count < ASH_LEVEL; count++)
	{
		if (count <= k0*average && count >= k1*STDEV && count <= k2*STDEV)
		{
			resultMap[count] = rounding(E * count);
			if (resultMap[count]>ASH_LEVEL - 1)
				resultMap[count] = ASH_LEVEL - 1;
		}
		else
		{
			resultMap[count] = count;
		}
	}

	unsigned int x, y;
	for (y = 0; y < HEIGHT; y++)
	{
		for (x = 0; x < WIDTH; x++)
		{
			THIS_ASH_LEVEL = resultMap[THIS_ASH_LEVEL];
		}
	}

	return rgbQuad;
}

/*��Ӧ����*/
BYTE* getresult_histogram(unsigned int *histogram, BYTE *rgbQuad)
{
	unsigned int x, y;
	for (y = 0; y < HEIGHT; y++)
	{
		for (x = 0; x < WIDTH; x++)
		{
			THIS_ASH_LEVEL = histogram[THIS_ASH_LEVEL];
		}
	}
	return rgbQuad;
}

/*�Ҷȷ�ת*/
BYTE* exchengeWhiteBlack(BYTE *rgbQuad)
{
	unsigned int x, y;
	for (y = 0; y < HEIGHT; y++)
	{
		for (x = 0; x < WIDTH; x++)
		{
			THIS_ASH_LEVEL = ASH_LEVEL - 1 - THIS_ASH_LEVEL;
		}
	}
	return rgbQuad;
}

/*٤��У��*/
BYTE* gammaAdjust(double gamma,BYTE *rgbQuad)
{	
	static BYTE gammaList[ASH_LEVEL];
	static double changeFlag = 0;
	static int first = 1;

	if (first)
		changeFlag = gamma;

	double ashResult;
	if (changeFlag != gamma || first)
	{
		changeFlag = gamma;
		for (unsigned int count = 0; count < ASH_LEVEL; count++)
		{
			ashResult = (count + 0.5) / ASH_LEVEL;//��һ��
			ashResult = pow(ashResult, 1.0 / gamma);//У��
			ashResult = ashResult * ASH_LEVEL - 0.5;//����һ��
			gammaList[count] = rounding(ashResult);//��������
		}
		if (first)
			first = 0;
	}

	unsigned int x, y;
	for (y = 0; y < HEIGHT; y++)
	{
		for (x = 0; x < WIDTH; x++)
		{
			THIS_ASH_LEVEL = gammaList[THIS_ASH_LEVEL];
		}
	}
	return rgbQuad;
}

/*����У��*/
BYTE* logAdjust(double coefficient, BYTE *rgbQuad)
{
	static BYTE gammaList[ASH_LEVEL];
	static double changeFlag = 0;
	static int first = 1;

	if (first)
		changeFlag = coefficient;

	double ashResult;
	if (changeFlag != coefficient || first)
	{
		changeFlag = coefficient;
		for (unsigned int count = 0; count < ASH_LEVEL; count++)
		{
			ashResult = coefficient*log(1.0 + count);
			if (ashResult<0)
				ashResult = 0;
			else if (ashResult>255)
				ashResult = 255;
			gammaList[count] = rounding(ashResult);//��������
		}
		if (first)
			first = 0;
	}

	unsigned int x, y;
	for (y = 0; y < HEIGHT; y++)
	{
		for (x = 0; x < WIDTH; x++)
		{
			THIS_ASH_LEVEL = gammaList[THIS_ASH_LEVEL];
		}
	}
	return rgbQuad;
}

/*�Ա�����*/
BYTE* drawContrast(BYTE ashMax, BYTE ashMin, BYTE* rgbQuad)
{
	static BYTE drawList[ASH_LEVEL];
	static int first = 1;
	static double changeFlag1 = 0;
	static double changeFlag2 = 0;
	static double k;

	if (first)
	{
		changeFlag1 = ashMax;
		changeFlag2 = ashMin;
	}

	double ashResult;
	if (first || (changeFlag1 != ashMax) || (changeFlag2 != ashMin))
	{
		k = (double)(ASH_LEVEL - 1) / (ashMax - ashMin);
		for (unsigned int count = ashMin; count <= ashMax; count++)
		{
				ashResult = k*(count - ashMin);
				drawList[count] = rounding(ashResult);
		}
		first = 0;
	}

	unsigned int x, y;
	for (y = 0; y < HEIGHT; y++)
	{
		for (x = 0; x < WIDTH; x++)
		{
			THIS_ASH_LEVEL = drawList[THIS_ASH_LEVEL];
		}
	}
	return rgbQuad;
}

/*�Ҷ��и�*/
BYTE* cutAsh(BYTE gray1, BYTE gray2, BYTE *rgbQuad)
{
	BYTE temp;
	temp = max(gray1, gray2);
	if (temp == gray1)
	{
		gray1 = gray2;
		gray2 = temp;
	}
	unsigned int x, y;
	for (y = 0; y < HEIGHT; y++)
	{
		for (x = 0; x < WIDTH; x++)
		{
			if (THIS_ASH_LEVEL >= gray1&&THIS_ASH_LEVEL <= gray2)
			{
				THIS_ASH_LEVEL = 0;
			}
			else
			{
				THIS_ASH_LEVEL = 255;
			}
		}
	}
	return rgbQuad;
}

/*λͼ�и�*/
BYTE* cutBitMap(BYTE level,BYTE *rgbQuad)
{
	static int levelMap[] = { 0, 1, 3, 7, 15, 31, 63, 127, 255 };
	unsigned int x, y;
	for (y = 0; y < HEIGHT; y++)
	{
		for (x = 0; x < WIDTH; x++)
		{
			if (THIS_ASH_LEVEL < levelMap[level] || THIS_ASH_LEVEL > levelMap[level + 1])
				THIS_ASH_LEVEL = 255;
		}
	}
	return rgbQuad;
}

/*��С����(������ɾ��)*/
BYTE* reduceTwice(BYTE *rgbQuad)
{
	DWORD newHeight, newWidth;
	newHeight = HEIGHT / 2;
	newWidth  = WIDTH / 2;
	BYTE *newDataMap = (BYTE*)malloc(sizeof(BYTE)*newHeight * newWidth);
	if (newDataMap == NULL)
	{
		printf("��Сͼ�������ڴ����ʧ��!\n");
		system("pause");
		exit(1);
	}
	unsigned int x, y;
	unsigned int i, j;
	for (y = 1, j = 0; y < HEIGHT&&j < newHeight; y += 2, j++)
	{
		for (x = 1, i = 0; x < WIDTH&&i < newWidth; x += 2, i++)
		{
			newDataMap[j*newWidth + i] = THIS_ASH_LEVEL;
		}
	}
	HEIGHT = newHeight;
	WIDTH  = newWidth;
	return newDataMap;
}

/*�Ŵ�����(�������ڲ�)*/
BYTE* magnifyTwice(BYTE *rgbQuad)
{
	DWORD newHeight, newWidth;
	newHeight = HEIGHT * 2;
	newWidth = WIDTH * 2;
	BYTE *newDataMap = (BYTE*)malloc(sizeof(BYTE)*newHeight * newWidth);
	if (newDataMap == NULL)
	{
		printf("��Сͼ�������ڴ����ʧ��!\n");
		system("pause");
		exit(1);
	}
	unsigned int x, y;
	unsigned int i, j;
	for (y = 0, j = 0; y < HEIGHT&&j < newHeight; y++, j += 2)
	{
		for (x = 0, i = 0; x < WIDTH&&i < newWidth; x++, i += 2)
		{
			newDataMap[j*newWidth + i] = THIS_ASH_LEVEL;
			newDataMap[(j + 1)*newWidth + i] = THIS_ASH_LEVEL;
			newDataMap[j*newWidth + (i + 1)] = THIS_ASH_LEVEL;
			newDataMap[(j + 1)*newWidth + (i + 1)] = THIS_ASH_LEVEL;
		}
	}
	HEIGHT = newHeight;
	WIDTH = newWidth;
	return newDataMap;
}

/*˫�����ڲ�ֵ(����)*/
BYTE* magnifyTwice_linear(BYTE *rgbQuad)
{
	DWORD newHeight, newWidth;
	double scaleH = 0.5;
	double scaleW = 0.5;
	newHeight = HEIGHT * 2;
	newWidth = WIDTH * 2;
	BYTE *newDataMap = (BYTE*)malloc(sizeof(BYTE)*newHeight * newWidth);
	if (newDataMap == NULL)
	{
		printf("��Сͼ�������ڴ����ʧ��!\n");
		system("pause");
		exit(1);
	}
	unsigned int dx, dy;//Ŀ������
	double sx, sy;//ԭ����
	double su, sv;//ԭ����С��λ
	for (dy = 0; dy < newHeight; dy++)
	{
		for (dx = 0; dx < newWidth; dx++)
		{
			sx = dx*scaleW; su = lnum(sx); sx = sx - su;
			sy = dy*scaleH; sv = lnum(sy); sy = sy - sv;
			newDataMap[dy*newWidth + dx] = \
				rounding(
				(1 - su)*(1 - sv)*rgbQuad[(int)(sy*WIDTH + sx)] + \
				(1 - su)*sv*rgbQuad[(int)((sy + 1)*WIDTH + sx)] + \
				su*(1 - sv)*rgbQuad[(int)(sy*WIDTH + (sx + 1))] + \
				su*sv*rgbQuad[(int)((sy + 1)*WIDTH + (sx + 1))]);
		}
	}
	HEIGHT = newHeight;
	WIDTH = newWidth;
	return newDataMap;
}

/*��Ƶ�����˲���(A=1ʱ��ԭΪ��׼������˹��)*/
BYTE* laplace(double A,BYTE *rgbQuad)
{
	BYTE *newDataMap = NULL;
	newDataMap = copyFrame(rgbQuad);
	unsigned int x, y;
	unsigned int temp;
	for (y = 1; y < HEIGHT - 1; y++)
	{
		for (x = 1; x < WIDTH - 1; x++)
		{
			temp =
				rounding(
				fabs(
				(4 + A) * THIS_ASH_LEVEL - \
				(rgbQuad[(y + 1)*WIDTH + x] + rgbQuad[y*WIDTH + (x + 1)] + \
				rgbQuad[(y - 1)*WIDTH + x] + rgbQuad[y*WIDTH + (x - 1)])));
			if (temp>255)
				newDataMap[COORDINATE] = 255;
			else
				newDataMap[COORDINATE] = temp;
		}
	}
	free(rgbQuad);
	rgbQuad = NULL;
	return newDataMap;
}

/*Sobel����(�ݶȱ任,���ڱ�Ե��ǿ)*/
BYTE* sobel(BYTE *rgbQuad)
{
	BYTE *newDataMap = NULL;
	newDataMap = copyFrame(rgbQuad);
	unsigned int x, y;
	int Gx, Gy, temp;
	for (y = 1; y < HEIGHT - 1; y++)
	{
		for (x = 1; x < WIDTH - 1; x++)
		{
			Gx = abs(
				rgbQuad[(y - 1)*WIDTH + (x - 1)] + 2 * rgbQuad[(y - 1)*WIDTH + x] + rgbQuad[(y - 1)*WIDTH + (x + 1)] -
				(rgbQuad[(y + 1)*WIDTH + (x - 1)] + 2 * rgbQuad[(y + 1)*WIDTH + x] + rgbQuad[(y + 1)*WIDTH + (x + 1)])
				);
			Gy = abs(
				rgbQuad[(y + 1)*WIDTH + (x + 1)] + 2 * rgbQuad[y*WIDTH + (x + 1)] + rgbQuad[(y - 1)*WIDTH + (x + 1)] -
				(rgbQuad[(y + 1)*WIDTH + (x - 1)] + 2 * rgbQuad[y*WIDTH + (x - 1)] + rgbQuad[(y - 1)*WIDTH + (x - 1)])
				);
			/*�����Ҫ���Ӿ�ȷ�Ĵ���,����ʹ��temp=sqrt(pow(Gx,2)+pow(Gy,2))*/
			if ((temp = Gx + Gy) > ASH_LEVEL - 1)
				newDataMap[COORDINATE] = ASH_LEVEL - 1;
			else
				newDataMap[COORDINATE] = temp;
		}
	}
	free(rgbQuad);
	rgbQuad = NULL;
	return newDataMap;
}

/*ƽ�������˲���(��Ȩƽ��)*/
BYTE* ave(BYTE *rgbQuad)
{
	BYTE *newDataMap = NULL;
	newDataMap = copyFrame(rgbQuad);
	unsigned int x, y;
	unsigned int temp;
	for (y = 1; y < HEIGHT - 1; y++)
	{
		for (x = 1; x < WIDTH - 1; x++)
		{
			temp =
				rgbQuad[(y + 1)*WIDTH + (x - 1)] + 2 * rgbQuad[(y + 1)*WIDTH + x] + rgbQuad[(y + 1)*WIDTH + (x + 1)] +
				2 * rgbQuad[y*WIDTH + (x - 1)] + 4 * rgbQuad[y*WIDTH + x] + 2 * rgbQuad[y*WIDTH + (x + 1)] +
				rgbQuad[(y - 1)*WIDTH + (x - 1)] + 2 * rgbQuad[(y - 1)*WIDTH + x] + rgbQuad[(y - 1)*WIDTH + (x + 1)];
			newDataMap[COORDINATE] = (temp >> 4);
		}
	}
	free(rgbQuad);
	rgbQuad = NULL;
	return newDataMap;
}

/*Ѱ����ֵ*/
BYTE getMedian(BYTE *data, int num)
{
	int i, j, temp;
	for (j = 0; j<num - 4; j++)
	for (i = 0; i<num - 1 - j; i++)
	{
		if (data[i]>data[i + 1])//����Ԫ�ش�С����������
		{
			temp = data[i];
			data[i] = data[i + 1];
			data[i + 1] = temp;
		}
	}
	return (data[num / 2 + 1]);
}

/*��ֵ�˲���*/
BYTE* median(BYTE *rgbQuad)
{
	BYTE *newDataMap = NULL;
	newDataMap = copyFrame(rgbQuad);
	BYTE data[9];
	unsigned int x, y;
	int s = 0;
	for (y = 1; y < HEIGHT - 1; y++)
	{
		for (x = 1; x < WIDTH - 1; x++)
		{
			data[0] = rgbQuad[(y + 1)*WIDTH + (x - 1)];
			data[1] = rgbQuad[(y + 1)*WIDTH + x];
			data[2] = rgbQuad[(y + 1)*WIDTH + (x + 1)];
			data[3] = rgbQuad[y*WIDTH + (x - 1)];
			data[4] = rgbQuad[y*WIDTH + x];
			data[5] = rgbQuad[y*WIDTH + (x + 1)];
			data[6] = rgbQuad[(y - 1)*WIDTH + (x - 1)];
			data[7] = rgbQuad[(y - 1)*WIDTH + x];
			data[8] = rgbQuad[(y - 1)*WIDTH + (x + 1)];
			newDataMap[COORDINATE] = getMedian(data, 9);
		}
	}
	free(rgbQuad);
	rgbQuad = NULL;
	return newDataMap;
}