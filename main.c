


#include "libpreprocessing/preprocessing/def.h"
#include "libpreprocessing/preprocessing/vmem.h"
#include "libpreprocessing/preprocessing/ana.h"
#include "libpreprocessing/preprocessing/arith.h"
#include "libpreprocessing/preprocessing/def_hough.h"
#include "libpreprocessing/preprocessing/hough.h"

#include "udp/udp.h"

/* from libeve */
#include "libeve/eve/fixed_point.h"

/* from std c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


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

	int status = PREPROCESSING_SUCCESSFUL;

	//Pendiente de poner en el define
	float Rmin=RADIO-WIDTH_RADIO/2;
	float Rmax=RADIO+WIDTH_RADIO/2;

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

	udp_createNANDFLASH(NANDFLASH, entriesOfNAND, stdimagesize, NUMBER_OF_IMAGES);
	//*************************************************************************************
	//END NAND FLASH Memory

	//	2.) Process image data:
	printf("Pipeline  start!\n");
	for(int index = 0; index < NUMBER_OF_IMAGES; index++){

		printf("--------------------------------\n");
		printf("Calculando Hough de la Imagen %d\n", index);
		printf("--------------------------------\n");

		//Read image i from NAND
		udp_loadImage(entriesOfNAND[index], ROWS, COLS, imgSdram);

		//Binarize image
		CHECK_STATUS(udp_binarize(imgSdram, tmp1Sdram, tmp2Sdram, ROWS, COLS, tmp2Sdram))

		for(double r=Rmin; r<Rmax; r+=STEP_HOUGH)
		{

			CHECK_STATUS( preprocessing_hough_accumulate(tmp2Sdram, tmp1Sdram, ROWS, COLS,
						eve_fp_int2s32(CENTER_DIST, FP32_FWL), eve_fp_double2s32(STEP_HOUGH, FP32_FWL),eve_fp_double2s32(r, FP32_FWL),
						(centers + index * (CENTERS_COLS)) ))
		}

		printf("Votes: %d    x: %d     y: %d\n", centers[index*CENTERS_COLS], centers[index*CENTERS_COLS+1], centers[index*CENTERS_COLS+2]);
	}


	printf("\n\n");
	for(int index = 0; index < NUMBER_OF_IMAGES; index++){
		int x = centers[index*CENTERS_COLS+1] - centers[1];
		int y = centers[index*CENTERS_COLS+2] - centers[2];

		printf("y: %d     x: %d\n", y, x);
	}

	printf("Done!\n");
    free(NANDFLASH);
    free(SDRAM);
	return status;

}
