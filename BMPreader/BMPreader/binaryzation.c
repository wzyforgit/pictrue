/*本文件的函数均需要在二值化的前提下进行操作*/

#include"Picture.h"

/*二值化*/
uint8* binaryzation(uint8 threshold, uint8 *rgbQuad)
{
	uint32 x, y;
	for (y = 0; y < HEIGHT; y++)
	{
		for (x = 0; x < WIDTH; x++)
		{
			if (THIS_ASH_LEVEL <= threshold)
			{
				BE_BLACK
			}
			else
			{
				BE_WHITE
			}
		}
	}
	return rgbQuad;
}

/*灰度反转*/
uint8* exchengeWhiteBlack_B(uint8 *rgbQuad)
{
	uint32 x, y;
	for (y = 0; y < HEIGHT; y++)
	{
		for (x = 0; x < WIDTH; x++)
		{
			THIS_ASH_LEVEL = ~THIS_ASH_LEVEL;
		}
	}
	return rgbQuad;
}

/*膨胀(基于二值化,取上下左右四个点)*/
uint8* expand(uint8 *rgbQuad)
{
	uint8 *newDataMap = NULL;
	newDataMap = copyFrame(rgbQuad);
	uint32 x, y;
	for (y = 1; y < HEIGHT - 1; y++)
	{
		for (x = 1; x < WIDTH - 1; x++)
		{
			newDataMap[COORDINATE] =
				THIS_ASH_LEVEL &&
				rgbQuad[ABOVE] &&
				rgbQuad[RIGHT] &&
				rgbQuad[BELOW] &&
				rgbQuad[LEFT];
		}
	}
	free(rgbQuad);
	rgbQuad = NULL;
	return newDataMap;
}

/*腐蚀(基于二值化,取上下左右四个点)*/
uint8* corrode(uint8 *rgbQuad)
{
	uint8 *newDataMap = NULL;
	newDataMap = copyFrame(rgbQuad);
	uint32 x, y;
	for (y = 1; y < HEIGHT - 1; y++)
	{
		for (x = 1; x < WIDTH - 1; x++)
		{
			newDataMap[COORDINATE] =
				(THIS_ASH_LEVEL ||
				rgbQuad[ABOVE] || 
				rgbQuad[RIGHT] ||
				rgbQuad[BELOW] || 
				rgbQuad[LEFT]);
		}
	}
	free(rgbQuad);
	rgbQuad = NULL;
	return newDataMap;
}

/*边界提取(取周围四个点)*/
uint8* getFrame(uint8 *rgbQuad)
{
	uint8 *newDataMap = NULL;
	newDataMap = copyFrame(rgbQuad);
	uint32 x, y;
	for (y = 1; y < HEIGHT - 1; y++)
	{
		for (x = 1; x < WIDTH - 1; x++)
		{
			newDataMap[COORDINATE] =
				(THIS_ASH_LEVEL & MASK_BOOL) ^     //如果先进行了膨胀处理,可以去掉掩码操作
				(THIS_ASH_LEVEL ||
				rgbQuad[ABOVE] || 
				rgbQuad[RIGHT] ||
				rgbQuad[BELOW] || 
				rgbQuad[LEFT]);
		}
	}
	free(rgbQuad);
	rgbQuad = NULL;
	return newDataMap;
}

/*拉普拉斯锐化*/
uint8* laplace_B(uint8 *rgbQuad)
{
	uint8 *newDataMap = NULL;
	newDataMap = copyFrame(rgbQuad);
	uint32 x, y;
	for (y = 1; y < HEIGHT - 1; y++)
	{
		for (x = 1; x < WIDTH - 1; x++)
		{
			newDataMap[COORDINATE] =
				(THIS_ASH_LEVEL ||
				rgbQuad[ABOVE] || 
				rgbQuad[RIGHT] ||
				rgbQuad[BELOW] || 
				rgbQuad[RIGHT]);
		}
	}
	free(rgbQuad);
	rgbQuad = NULL;
	return newDataMap;
}