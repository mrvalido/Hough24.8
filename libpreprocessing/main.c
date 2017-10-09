


#include "preprocessing/def.h"
#include "preprocessing/mydef.h"
#include "preprocessing/vmem.h"
#include "preprocessing/ana.h"
#include "preprocessing/arith.h"
#include "preprocessing/hough.h"



/* from libeve */
#include "../libeve/eve/fixed_point.h"

/* from std c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "FITS_Interface.h"

void writeImageToFile(int32_t *img, char *fileName, int positionIndex,  int index, uint32_t stdimagesize ){
	FILE *fp;
	char file[20];
	strcpy(file,fileName);

	if(index < 10){
		file[positionIndex] = 48 + index;
	}else{
		int decimal = index/10;
		int subDecimal = index-decimal*10;
		file[positionIndex-1] = 48 + decimal;
		file[positionIndex] = 48 + subDecimal;
	}

	fp=fopen(file,"wb");
	fwrite(img,sizeof(int32_t),stdimagesize,fp);
	fclose(fp);
}

/*
 * * * * * * *
 * * MAIN  * *
 * * * * * * *
 */

int main()
{
	int32_t *SDRAM;
	int32_t *img;
	int32_t *centers;
	int32_t *tmp1;
	int32_t *tmp2;

	uint32_t stdimagesize=ROWS*COLS;
	uint32_t stdCentersSize = CENTERS_ROWS*CENTERS_COLS;

	//Pendiente de poner en el define
	float Rmin=RADIO-WIDTH_RADIO/2;
	float Rmax=RADIO+WIDTH_RADIO/2;

	unsigned int Xmin=XC-CENTER_DIST; 	// Xmin and Xmax define a square boundaries of coordinates around of CCD center (XC,YC)
	unsigned int Xmax=XC+CENTER_DIST;
	unsigned int Ymin=YC-CENTER_DIST; 	// Xmin and Xmax define a square boundaries of coordinates around of CCD center (XC,YC)
	unsigned int Ymax=YC+CENTER_DIST;

	printf ("Start!\n");

	/*
	 * Memory allocation
	 * Corresponds to part of copying images to SDRAM, total size of virtual RAM
	 */
	SDRAM = (int32_t*) malloc((uint32_t)NUMBER_ENTRIES_SDRAM*stdimagesize*sizeof(int32_t));

	/*
	 * * * * * * * * *
	 * memory mapping*
	 * * * * * * * * *
	 */

	//	1.) Load image data to (virtual) SDRAM:
	uint32_t 	imgSdram = 0;
	uint32_t 	imgSize = stdimagesize;
	uint32_t 	imgDatasetId = 1;

	uint32_t    centersSdram = imgSdram + imgSize;
	uint32_t	centersSize  = stdCentersSize;
	uint32_t	centersDatasetId = 2;

	uint32_t	tmp1Sdram = centersSdram + centersSize;
	uint32_t	tmp1Size = stdimagesize;
	uint32_t	tmp1DatasetId = 3;

	uint32_t	tmp2Sdram = tmp1Sdram + tmp1Size;
	uint32_t	tmp2Size = stdimagesize;
	uint32_t	tmp2DatasetId = 4;

	img=(SDRAM+imgSdram);
	centers=(SDRAM+centersSdram);
	tmp1=(SDRAM+tmp1Sdram);
	tmp2=(SDRAM+tmp2Sdram);

	preprocessing_vmem_setEntry(imgSdram, imgSize, imgDatasetId, img);
	preprocessing_vmem_setEntry(centersSdram, centersSize, centersDatasetId, centers);
	preprocessing_vmem_setEntry(tmp1Sdram, tmp1Size, tmp1DatasetId, tmp1);
	preprocessing_vmem_setEntry(tmp2Sdram, tmp2Size, tmp2DatasetId, tmp2);

	preprocessing_vmem_print();


	//NAND FLASH Memory
	//*************************************************************************************
	int32_t *NANDFLASH;
	int32_t numberOfEntriesNAND = 128;
	int32_t **entriesOfNAND = (int32_t **) malloc(numberOfEntriesNAND*sizeof(int32_t *));
	NANDFLASH = (int32_t*) malloc(numberOfEntriesNAND*stdimagesize*sizeof(int32_t));

	createNANDFLASH(NANDFLASH, entriesOfNAND, stdimagesize, NUMBER_OF_IMAGES);
	//*************************************************************************************
	//END NAND FLASH Memory

	//	2.) Process image data:
	printf("Pipeline  start!\n");
	for(int index = 0; index < NUMBER_OF_IMAGES; index++){

		printf("--------------------------------\n");
		printf("Calculando Hough de la Imagen %d\n", index);
		printf("--------------------------------\n");

		//Read image i from NAND
		readNAND(entriesOfNAND[index], ROWS, COLS, imgSdram);

		//Calculate DX
		preprocessing_zero(tmp1Sdram, ROWS, COLS, tmp1Sdram);
		preprocessing_ana_deriveX(imgSdram,ROWS, COLS,tmp1Sdram);
		preprocessing_arith_abs(tmp1Sdram,ROWS, COLS,tmp1Sdram);

		//Calculate DY
		preprocessing_zero(tmp2Sdram, ROWS, COLS, tmp2Sdram);
		preprocessing_ana_deriveY(imgSdram,ROWS, COLS,tmp2Sdram);
		preprocessing_arith_abs(tmp2Sdram,ROWS, COLS,tmp2Sdram);

		//Calculate Sum DX & DY
		preprocessing_arith_addImages(tmp1Sdram, tmp2Sdram, ROWS, COLS, tmp2Sdram);

		//Calculate border
		preprocessing_arith_subtractImages(tmp2Sdram, imgSdram, ROWS, COLS, tmp2Sdram);
		preprocessing_ana_overThresh(tmp2Sdram, ROWS, COLS, 0, tmp2Sdram);

		for(float r=Rmin; r<Rmax; r+=STEP_HOUGH)
		{
			preprocessing_zero(tmp1Sdram, ROWS, COLS, tmp1Sdram);				//Reset Accumulator
			preprocessing_hough(tmp2Sdram, ROWS, COLS, Xmin,Xmax,Ymin,Ymax,r*r,STEP_HOUGH,tmp1Sdram);

			//preprocessing_ana_median(tmp1Sdram,rows, cols,tmp2Sdram); //It is here to check if it improve something
			preprocessing_maximumValue(tmp1Sdram, ROWS, COLS, CENTER_DIST, STEP_HOUGH, index, centersSdram);
		}

		printf("Votes: %d    x: %d     y: %d\n", centers[index*CENTERS_COLS], centers[index*CENTERS_COLS+1], centers[index*CENTERS_COLS+2]);
	}





	printf("Done!\n");
	return 1;

}
