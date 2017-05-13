#include"picture.h"
#include"ansiLib.h"

/*�ҶȻ�(��Ȩƽ����)*/
dm8* getAsh(picHead *h, dm24 *dataMap)
{
	int x, y;
	dm8* ashMap;
	GETSIZE();
	ashMap = (dm8*)malloc(sizeof(dm8)*width*height);
	if (ashMap == NULL)
	{
		printf("�Ҷ�ͼ��ɫ���ڴ�����ʧ��!\n");
		system("pause");
		exit(1);
	}
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			ashMap[COORDINATE] =
				(dataMap[COORDINATE].blue * 11 + dataMap[COORDINATE].green * 59 + dataMap[COORDINATE].red * 30) / 100;
		}
	}
	h->infoHeader.biBitCount = 8;
	return ashMap;
}

/*���Ʊ߿�*/
static dm8* copyFrame(picHead *h, dm8 *dataMap)
{
	GETSIZE();
	/*Ƶ��malloc���ܵ����ڴ���Ƭ���������,��ʱ�޷����*/
	dm8 *newDataMap = (dm8*)malloc(sizeof(dm8)* height * width);
	if (newDataMap == NULL)
	{
		printf("���Ʊ߿��ڴ����ʧ��\n");
		system("pause");
		exit(1);
	}
	/*���Ʊ߿�*/
	int x, y;
	y = 0;
	for (x = 0; x < width; x++)
	{
		//		newDataMap[COORDINATE] = THIS_ASH_LEVEL;
		newDataMap[COORDINATE] = 0;
	}
	for (y = 1; y < height - 1; y++)
	{
		x = 0;
		//		newDataMap[COORDINATE] = THIS_ASH_LEVEL;
		newDataMap[COORDINATE] = 0;
		x = x + width - 1;
		//		newDataMap[COORDINATE] = THIS_ASH_LEVEL;
		newDataMap[COORDINATE] = 0;
	}
	for (x = 0; x < width; x++)
	{
		//		newDataMap[COORDINATE] = THIS_ASH_LEVEL;
		newDataMap[COORDINATE] = 0;
	}
	return newDataMap;
}

/*��ȡ�Ҷ�ͼ��ֱ��ͼ*/
uint32* getHistogram(picHead *h, dm8 *dataMap)
{
	int x, y;
	uint32 *histogram = (uint32*)malloc(sizeof(uint32)* ASH_LEVEL);
	if (histogram == NULL)
	{
		printf("ֱ��ͼ�ڴ����ʧ��!\n");
		system("pause");
		exit(1);
	}
	memset(histogram, 0, sizeof(uint32)*ASH_LEVEL);
	GETSIZE();
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			histogram[dataMap[COORDINATE]]++;
		}
	}
	return histogram;
}

/*�Ҷ�ƽ��ֵ*/
uint32 getAshAverage(picHead *h, uint32 *histogram)
{
	uint32 sum = 0;
	uint32 count;
	GETSIZE();
	for (count = 0; count < 256; count++)
	{
		sum += count * histogram[count];
	}
	return sum / (width*height);
}

/*�Ҷȱ�׼��*/
float64 getAshSTDEV(picHead *h,uint32 average, uint32 *histogram)
{
	float64 sum = 0;
	float64 EVmap[ASH_LEVEL];
	GETSIZE();
	for (int count = 0; count < ASH_LEVEL; count++)
	{
		EVmap[count] = pow(count - (float64)average, 2);
	}

	for (int count = 0; count < ASH_LEVEL; count++)
	{
		sum += (histogram[count] * EVmap[count]);
	}
	return (sqrt(sum / (height*width)));
}

/*������ǿ*/
/*һ��ȡE=4.0,K0=0.4,K1=0.02,K2=0.4,���Ը���ʵ���������һ������*/
dm8* areaUp(float64 E, float64 k0, float64 k1, float64 k2, picHead *h, dm8 *dataMap)
{
	dm8 resultMap[ASH_LEVEL];
	GETSIZE();
	uint32 *histogram = NULL;
	histogram = getHistogram(h, dataMap);
	uint32 average = getAshAverage(h, histogram);
	float64 STDEV = getAshSTDEV(h, average, histogram);

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

	int x, y;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			THIS_ASH_LEVEL = resultMap[THIS_ASH_LEVEL];
		}
	}
	return dataMap;
}

/*��ȡ���,��С�Ҷ�ֵ*/
void getAshMaxMin(uint8 *ashMax, uint8 *ashMin, uint32 *histogram)
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
uint32* balanceHistogram(picHead *h,uint32 *histogram)
{
	uint32 x, y;
	float64 pHistoGram[ASH_LEVEL];//���ָ���,��һ��ֱ��ͼ
	float64 cHistoGram[ASH_LEVEL];//�ۻ���һ��ֱ��ͼ(���⻯���)
	GETSIZE();
	for (int count = 0; count < ASH_LEVEL; count++)//��һ��
	{
		pHistoGram[count] = (float64)histogram[count] / (width*height);
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
/*uint32* matchHistogram(picHead *h,dm8 *source,dm8 *aim,uint32 *histogram, uint32 *goal)
{
	GETSIZE();
	//uint32 average = getAshAverage(h, goal);
	histogram = balanceHistogram(h, histogram);
	goal = balanceHistogram(h, goal);
	uint32* temp_hist = (uint32*)malloc(sizeof(uint32)*ASH_LEVEL);
	uint32* temp_goal = (uint32*)malloc(sizeof(uint32)*ASH_LEVEL);
	memset(temp_hist, 0, sizeof(uint32)*ASH_LEVEL);
	memset(temp_goal, 0, sizeof(uint32)*ASH_LEVEL);
	int32 x, y;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			temp_hist[histogram[source[y*width + x]]]++;
			temp_goal[goal[aim[y*width + x]]]++;
		}
	}
	uint8 difference, temp, temps;
	for (y = 0; y < ASH_LEVEL; y++)
	{
		for (x = 0, temp = ASH_LEVEL - 1; x < ASH_LEVEL; x++)
		{
			if ((difference = abs(temp_hist[y] - temp_goal[x]) == 0))
			{
				temp_hist[y] = x;
				break;
			}
			if (difference < temp)
			{
				temp = difference;
				temps = x;
			}
		}
		if (x == ASH_LEVEL)
		{
			temp_hist[y] = temps;
			//histogram[y] = average;
		}
	}
	return temp_hist;
}*/

/*��Ӧ����*/
dm8* getresult_histogram(uint32 *histogram, picHead *h, dm8 *dataMap)
{
	int x, y;
	GETSIZE();
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			THIS_ASH_LEVEL = (dm8)histogram[THIS_ASH_LEVEL];
		}
	}
	return dataMap;
}

/*�Ҷȷ�ת*/
dm8* exchengeWhiteBlack(picHead *h, dm8 *dataMap)
{
	int x, y;
	GETSIZE();
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			THIS_ASH_LEVEL = ASH_LEVEL - 1 - THIS_ASH_LEVEL;
		}
	}
	return dataMap;
}

/*٤��У��*/
dm8* gammaAdjust(float64 gamma, picHead *h, dm8 *dataMap)
{
	static uint8 gammaList[ASH_LEVEL];
	static float64 changeFlag = 0;
	static int first = 1;
	float64 ashResult;
	GETSIZE();

	if (first)
		changeFlag = gamma;

	if (changeFlag != gamma || first)
	{
		changeFlag = gamma;
		for (uint32 count = 0; count < ASH_LEVEL; count++)
		{
			ashResult = (count + 0.5) / ASH_LEVEL;//��һ��
			ashResult = pow(ashResult, 1.0 / gamma);//У��
			ashResult = ashResult * ASH_LEVEL - 0.5;//����һ��
			gammaList[count] = rounding(ashResult);//��������
		}
		if (first)
			first = 0;
	}

	int32 x, y;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			THIS_ASH_LEVEL = gammaList[THIS_ASH_LEVEL];
		}
	}
	return dataMap;
}

/*����У��*/
dm8* logAdjust(float64 coefficient, picHead *h, dm8 *dataMap)
{
	static uint8 logList[ASH_LEVEL];
	static float64 changeFlag = 0;
	static int first = 1;
	float64 ashResult;
	GETSIZE();
	if (first)
		changeFlag = coefficient;

	if (changeFlag != coefficient || first)
	{
		changeFlag = coefficient;
		for (uint32 count = 0; count < ASH_LEVEL; count++)
		{
			ashResult = coefficient*log(1.0 + count);
			if (ashResult<0)
				ashResult = 0;
			else if (ashResult>255)
				ashResult = 255;
			logList[count] = rounding(ashResult);//��������
		}
		if (first)
			first = 0;
	}

	int32 x, y;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			THIS_ASH_LEVEL = logList[THIS_ASH_LEVEL];
		}
	}
	return dataMap;
}

/*�Ա�����*/
dm8* drawContrast(picHead *h, dm8* dataMap)
{
	static uint8 drawList[ASH_LEVEL];
	static int first = 1;
	static float64 changeFlag1 = 0;
	static float64 changeFlag2 = 0;
	static float64 k;
	GETSIZE();

	uint32 *histogram = NULL;
	histogram = getHistogram(h, dataMap);
	uint8 ashMax, ashMin;
	getAshMaxMin(&ashMax, &ashMin, histogram);

	if (first)
	{
		changeFlag1 = ashMax;
		changeFlag2 = ashMin;
	}

	float64 ashResult;
	if (first || (changeFlag1 != ashMax) || (changeFlag2 != ashMin))
	{
		k = (float64)(ASH_LEVEL - 1) / (ashMax - ashMin);
		for (uint32 count = ashMin; count <= ashMax; count++)
		{
			ashResult = k*(count - ashMin);
			drawList[count] = rounding(ashResult);
		}
		first = 0;
	}

	int32 x, y;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			THIS_ASH_LEVEL = drawList[THIS_ASH_LEVEL];
		}
	}
	return dataMap;
}

/*�Ҷ��и�*/
dm8* cutAsh(uint8 gray1, uint8 gray2, picHead *h, dm8 *dataMap)
{
	uint8 temp;
	temp = max(gray1, gray2);
	if (temp == gray1)
	{
		gray1 = gray2;
		gray2 = temp;
	}
	GETSIZE();
	int32 x, y;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
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
	return dataMap;
}

/*λͼ�и�*/
dm8* cutBitMap(uint8 level, picHead *h, dm8 *dataMap)
{
	static int levelMap[] = { 0, 1, 3, 7, 15, 31, 63, 127, 255 };
	GETSIZE();
	int32 x, y;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			if (THIS_ASH_LEVEL < levelMap[level] || THIS_ASH_LEVEL > levelMap[level + 1])
				THIS_ASH_LEVEL = 255;
		}
	}
	return dataMap;
}

/*��С����(������ɾ��)*/
dm8* reduceTwice(picHead *h, dm8 *dataMap)
{
	DWORD newHeight, newWidth;
	GETSIZE();
	newHeight = height / 2;
	newWidth = width / 2;
	dm8 *newDataMap = (dm8*)malloc(sizeof(dm8)*newHeight * newWidth);
	if (newDataMap == NULL)
	{
		printf("��Сͼ�������ڴ����ʧ��!\n");
		system("pause");
		exit(1);
	}
	int32 x, y;
	uint32 i, j;
	for (y = 1, j = 0; y < height&&j < newHeight; y += 2, j++)
	{
		for (x = 1, i = 0; x < width&&i < newWidth; x += 2, i++)
		{
			newDataMap[j*newWidth + i] = THIS_ASH_LEVEL;
		}
	}
	h->infoHeader.biHeight = newHeight;
	h->infoHeader.biWidth = newWidth;
	free(dataMap);
	dataMap = NULL;
	return newDataMap;
}

/*�Ŵ�����(�������ڲ�)*/
dm8* magnifyTwice(picHead *h, dm8 *dataMap)
{
	DWORD newHeight, newWidth;
	GETSIZE();
	newHeight = height * 2;
	newWidth = width * 2;
	dm8 *newDataMap = (dm8*)malloc(sizeof(dm8)*newHeight * newWidth);
	if (newDataMap == NULL)
	{
		printf("��Сͼ�������ڴ����ʧ��!\n");
		system("pause");
		exit(1);
	}
	int32 x, y;
	uint32 i, j;
	for (y = 0, j = 0; y < height&&j < newHeight; y++, j += 2)
	{
		for (x = 0, i = 0; x < width&&i < newWidth; x++, i += 2)
		{
			newDataMap[j*newWidth + i] = THIS_ASH_LEVEL;
			newDataMap[(j + 1)*newWidth + i] = THIS_ASH_LEVEL;
			newDataMap[j*newWidth + (i + 1)] = THIS_ASH_LEVEL;
			newDataMap[(j + 1)*newWidth + (i + 1)] = THIS_ASH_LEVEL;
		}
	}
	h->infoHeader.biHeight = newHeight;
	h->infoHeader.biWidth = newWidth;
	free(dataMap);
	dataMap = NULL;
	return newDataMap;
}

/*˫�����ڲ�ֵ(����)*/
dm8* magnifyTwice_linear(picHead *h, dm8 *dataMap)
{
	DWORD newHeight, newWidth;
	GETSIZE();
	float64 scaleH = 0.5;
	float64 scaleW = 0.5;
	newHeight = height * 2;
	newWidth = width * 2;
	dm8 *newDataMap = (dm8*)malloc(sizeof(dm8)*newHeight * newWidth);
	if (newDataMap == NULL)
	{
		printf("��Сͼ�������ڴ����ʧ��!\n");
		system("pause");
		exit(1);
	}
	uint32 dx, dy;//Ŀ������
	float64 sx, sy;//ԭ����
	float64 su, sv;//ԭ����С��λ
	for (dy = 0; dy < newHeight; dy++)
	{
		for (dx = 0; dx < newWidth; dx++)
		{
			sx = dx*scaleW; su = lnum(sx); sx = sx - su;
			sy = dy*scaleH; sv = lnum(sy); sy = sy - sv;
			newDataMap[dy*newWidth + dx] = \
				rounding(
				(1 - su)*(1 - sv)*dataMap[(int)(sy*width + sx)] + \
				(1 - su)*sv*dataMap[(int)((sy + 1)*width + sx)] + \
				su*(1 - sv)*dataMap[(int)(sy*width + (sx + 1))] + \
				su*sv*dataMap[(int)((sy + 1)*width + (sx + 1))]);
		}
	}
	h->infoHeader.biHeight = newHeight;
	h->infoHeader.biWidth = newWidth;
	free(dataMap);
	dataMap = NULL;
	return newDataMap;
}

/*��Ƶ�����˲���(A=1ʱ��ԭΪ��׼������˹��)*/
dm8* laplace(float64 A, picHead *h, dm8 *dataMap)
{
	dm8 *newDataMap = NULL;
	GETSIZE();
	newDataMap = copyFrame(h, dataMap);
	int32 x, y;
	int32 temp;
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			temp =
				rounding(
				fabs(
				(4 + A) * THIS_ASH_LEVEL - \
				(dataMap[(y + 1)*width + x] + dataMap[y*width + (x + 1)] + \
				dataMap[(y - 1)*width + x] + dataMap[y*width + (x - 1)])));
			if (temp>255)
				newDataMap[COORDINATE] = 255;
			else
				newDataMap[COORDINATE] = temp;
		}
	}
	free(dataMap);
	dataMap = NULL;
	return newDataMap;
}

/*Sobel����(�ݶȱ任,���ڱ�Ե��ǿ)*/
dm8* sobel(picHead *h, dm8 *dataMap)
{
	dm8 *newDataMap = NULL;
	GETSIZE();
	newDataMap = copyFrame(h, dataMap);
	int32 x, y;
	int Gx, Gy, temp;
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			Gx = abs(
				dataMap[(y - 1)*width + (x - 1)] + 2 * dataMap[(y - 1)*width + x] + dataMap[(y - 1)*width + (x + 1)] -
				(dataMap[(y + 1)*width + (x - 1)] + 2 * dataMap[(y + 1)*width + x] + dataMap[(y + 1)*width + (x + 1)])
				);
			Gy = abs(
				dataMap[(y + 1)*width + (x + 1)] + 2 * dataMap[y*width + (x + 1)] + dataMap[(y - 1)*width + (x + 1)] -
				(dataMap[(y + 1)*width + (x - 1)] + 2 * dataMap[y*width + (x - 1)] + dataMap[(y - 1)*width + (x - 1)])
				);
			/*�����Ҫ���Ӿ�ȷ�Ĵ���,����ʹ��temp=sqrt(pow(Gx,2)+pow(Gy,2))*/
			if ((temp = Gx + Gy) > ASH_LEVEL - 1)
				newDataMap[COORDINATE] = ASH_LEVEL - 1;
			else
				newDataMap[COORDINATE] = temp;
		}
	}
	free(dataMap);
	dataMap = NULL;
	return newDataMap;
}

/*ƽ�������˲���(��Ȩƽ��)*/
dm8* ave(picHead *h, dm8 *dataMap)
{
	dm8 *newDataMap = NULL;
	GETSIZE();
	newDataMap = copyFrame(h, dataMap);
	int32 x, y;
	int32 temp;
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			temp =
				dataMap[(y + 1)*width + (x - 1)] + 2 * dataMap[(y + 1)*width + x] + dataMap[(y + 1)*width + (x + 1)] +
				2 * dataMap[y*width + (x - 1)] + 4 * dataMap[y*width + x] + 2 * dataMap[y*width + (x + 1)] +
				dataMap[(y - 1)*width + (x - 1)] + 2 * dataMap[(y - 1)*width + x] + dataMap[(y - 1)*width + (x + 1)];
			newDataMap[COORDINATE] = (temp >> 4);
		}
	}
	free(dataMap);
	dataMap = NULL;
	return newDataMap;
}

/*Ѱ����ֵ*/
uint8 getMedian(uint8 *data, int num)
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
dm8* median(picHead *h, dm8 *dataMap)
{
	dm8 *newDataMap = NULL;
	GETSIZE();
	newDataMap = copyFrame(h, dataMap);
	int8 data[9];
	int32 x, y;
	int s = 0;
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			data[0] = dataMap[(y + 1)*width + (x - 1)];
			data[1] = dataMap[(y + 1)*width + x];
			data[2] = dataMap[(y + 1)*width + (x + 1)];
			data[3] = dataMap[y*width + (x - 1)];
			data[4] = dataMap[y*width + x];
			data[5] = dataMap[y*width + (x + 1)];
			data[6] = dataMap[(y - 1)*width + (x - 1)];
			data[7] = dataMap[(y - 1)*width + x];
			data[8] = dataMap[(y - 1)*width + (x + 1)];
			newDataMap[COORDINATE] = getMedian(data, 9);
		}
	}
	free(dataMap);
	dataMap = NULL;
	return newDataMap;
}