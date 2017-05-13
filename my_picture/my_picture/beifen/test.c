#include"picture.h"
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

void* getDataMap(char* fileName, picHead *h);
int main(void)
{
	clock_t start, end;
	void *ash = NULL;
	picHead head;
	ash = getDataMap("ssss.bmp", &head);
	start = clock();
	/*********************start*********************/



	/**********************end**********************/
	end = clock();
	savePicture_24bit("test1.bmp", &head, ash);
	system("start ssss.bmp");
	printf("time: %ldms\n", end - start);
	return 0;
}