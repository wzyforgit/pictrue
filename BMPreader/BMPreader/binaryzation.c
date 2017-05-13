/*���ļ��ĺ�������Ҫ�ڶ�ֵ����ǰ���½��в���*/

#include"Picture.h"

/*��ֵ��*/
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

/*�Ҷȷ�ת*/
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

/*����(���ڶ�ֵ��,ȡ���������ĸ���)*/
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

/*��ʴ(���ڶ�ֵ��,ȡ���������ĸ���)*/
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

/*�߽���ȡ(ȡ��Χ�ĸ���)*/
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
				(THIS_ASH_LEVEL & MASK_BOOL) ^     //����Ƚ��������ʹ���,����ȥ���������
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

/*������˹��*/
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