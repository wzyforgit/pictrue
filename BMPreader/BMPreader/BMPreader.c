/*�Ա�����*/
/*	BYTE ashMax, ashMin;
unsigned int histogram[256]={0};
getHistogram(histogram, ash);
getAshMaxMin(&ashMax, &ashMin, histogram);
ash=drawContrast(ashMax, ashMin, ash);
savePicture_ash("test1.bmp", ash);*/

#include"Picture.h"
#include<time.h>
#include<string.h>
#include<math.h>

typedef struct
{
	int x;
	int y;
}Site;

int main(void)
{
	/*��ʼ���洢�������ݵ��ڴ�*/
	RGB_QUAD *color = NULL;
	/*��ȡ��������*/
	color = getDataMap("ssss.bmp", color);
	/*�ҶȻ�*/
	BYTE *ash = NULL;
	ash = getAsh(color);
	free(color);
	clock_t start, end;
	start = clock();
	/********************************************************************************************/
#define kx -0.00000550
#define ky -0.00000550
#include"123.txt"
	BYTE *newmap = (BYTE*)malloc(sizeof(BYTE)*WIDTH*HEIGHT);
	memset(newmap, 5, sizeof(BYTE)*WIDTH*HEIGHT);
	int x, y;
	for (y = 0;y < HEIGHT;y++)
	{
		for (x = 0;x < WIDTH;x++)
		{
			newmap[x + y*WIDTH] = ash[adjust_goal[x + y*WIDTH].x + (adjust_goal[x + y*WIDTH].y)*WIDTH];
		}
	}
	/********************************************************************************************/
	end = clock();
	savePicture_ash("test2.bmp", newmap);
	system("start test2.bmp");
	printf("%ldms\n", end - start);
	return 0;
}