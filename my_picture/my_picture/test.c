#include"picture.h"
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(void)
{
	clock_t start, end;
	void *ash = NULL;
	picHead head;
	ash = getDataMap("ssss.bmp", &head);
	ash = getAsh(&head, ash);
	savePicture("test1.bmp", &head, ash);
	start = clock();
	/*********************start*********************/

	ash = gammaAdjust(4, &head, ash);

	/**********************end**********************/
	end = clock();
	savePicture("test2.bmp", &head, ash);
	system("start test2.bmp");
	printf("time: %ldms\n", end - start);
	return EXIT_SUCCESS;
}