#include"picture.h"
#include"ansiLib.h"

/*灰度化(加权平均法)*/
dm8* getAsh(picHead *h, dm24 *dataMap)
{
	int x, y;
	dm8* ashMap;
	GETSIZE();
	ashMap = (dm8*)malloc(sizeof(dm8)*width*height);
	if (ashMap == NULL)
	{
		printf("灰度图调色板内存申请失败!\n");
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

/*复制边框*/
static dm8* copyFrame(picHead *h, dm8 *dataMap)
{
	GETSIZE();
	/*频繁malloc可能导致内存碎片过多而崩溃,暂时无法解决*/
	dm8 *newDataMap = (dm8*)malloc(sizeof(dm8)* height * width);
	if (newDataMap == NULL)
	{
		printf("复制边框内存分配失败\n");
		system("pause");
		exit(1);
	}
	/*复制边框*/
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

/*获取灰度图像直方图*/
uint32* getHistogram(picHead *h, dm8 *dataMap)
{
	int x, y;
	uint32 *histogram = (uint32*)malloc(sizeof(uint32)* ASH_LEVEL);
	if (histogram == NULL)
	{
		printf("直方图内存分配失败!\n");
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

/*灰度平均值*/
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

/*灰度标准差*/
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

/*区域增强*/
/*一般取E=4.0,K0=0.4,K1=0.02,K2=0.4,可以根据实际情况作进一步调整*/
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

/*获取最大,最小灰度值*/
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

/*直方图均衡化*/
uint32* balanceHistogram(picHead *h,uint32 *histogram)
{
	uint32 x, y;
	float64 pHistoGram[ASH_LEVEL];//出现概率,归一化直方图
	float64 cHistoGram[ASH_LEVEL];//累积归一化直方图(均衡化结果)
	GETSIZE();
	for (int count = 0; count < ASH_LEVEL; count++)//归一化
	{
		pHistoGram[count] = (float64)histogram[count] / (width*height);
	}
	for (y = 0; y < ASH_LEVEL; y++)//均衡化
	{
		cHistoGram[y] = 0;
		for (x = 0; x <= y; x++)
		{
			cHistoGram[y] += pHistoGram[x];
		}
	}
	for (int count = 0; count < ASH_LEVEL; count++)//计算最终结果
	{
		histogram[count] = rounding(cHistoGram[count] * 255);
	}

	return histogram;
}

/*直方图规定化*/
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

/*对应计算*/
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

/*灰度反转*/
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

/*伽玛校正*/
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
			ashResult = (count + 0.5) / ASH_LEVEL;//归一化
			ashResult = pow(ashResult, 1.0 / gamma);//校正
			ashResult = ashResult * ASH_LEVEL - 0.5;//反归一化
			gammaList[count] = rounding(ashResult);//四舍五入
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

/*对数校正*/
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
			logList[count] = rounding(ashResult);//四舍五入
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

/*对比拉伸*/
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

/*灰度切割*/
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

/*位图切割*/
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

/*缩小两倍(最邻域删除)*/
dm8* reduceTwice(picHead *h, dm8 *dataMap)
{
	DWORD newHeight, newWidth;
	GETSIZE();
	newHeight = height / 2;
	newWidth = width / 2;
	dm8 *newDataMap = (dm8*)malloc(sizeof(dm8)*newHeight * newWidth);
	if (newDataMap == NULL)
	{
		printf("缩小图像像素内存分配失败!\n");
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

/*放大两倍(最领域内插)*/
dm8* magnifyTwice(picHead *h, dm8 *dataMap)
{
	DWORD newHeight, newWidth;
	GETSIZE();
	newHeight = height * 2;
	newWidth = width * 2;
	dm8 *newDataMap = (dm8*)malloc(sizeof(dm8)*newHeight * newWidth);
	if (newDataMap == NULL)
	{
		printf("缩小图像像素内存分配失败!\n");
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

/*双线性内插值(两倍)*/
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
		printf("缩小图像像素内存分配失败!\n");
		system("pause");
		exit(1);
	}
	uint32 dx, dy;//目标坐标
	float64 sx, sy;//原坐标
	float64 su, sv;//原坐标小数位
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

/*高频提升滤波器(A=1时还原为标准拉普拉斯锐化)*/
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

/*Sobel算子(梯度变换,用于边缘增强)*/
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
			/*如果需要更加精确的处理,可以使用temp=sqrt(pow(Gx,2)+pow(Gy,2))*/
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

/*平滑线性滤波器(加权平均)*/
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

/*寻找中值*/
uint8 getMedian(uint8 *data, int num)
{
	int i, j, temp;
	for (j = 0; j<num - 4; j++)
	for (i = 0; i<num - 1 - j; i++)
	{
		if (data[i]>data[i + 1])//数组元素大小按升序排列
		{
			temp = data[i];
			data[i] = data[i + 1];
			data[i + 1] = temp;
		}
	}
	return (data[num / 2 + 1]);
}

/*中值滤波器*/
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