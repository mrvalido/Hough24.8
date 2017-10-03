


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


/*
 * * * * * * *
 * * MAIN  * *
 * * * * * * *
 */


int main()
{


	int32_t *SDRAM;
	int32_t *img01;
	int32_t *img02;
	int32_t *img03;
	int32_t *img04;
	int32_t *img05;
	int32_t *img06;
	int32_t *img07;
	int32_t *img08;
	int32_t *img09;
	int32_t *centers;
	int32_t *mean;
	int32_t *tmp1;
	int32_t *tmp2;
	int32_t *tmp3;


	int32_t *min, *max;
	int32_t M=0;
	int32_t m=0;
	int32_t Scale;
	int32_t threshold=0;
	int32_t newThresh=0;
	double dif_threshold;
	Scale=eve_fp_int2s32(255, FP32_FWL );
	max=&M;
	min=&m;
	int *inputimg;
	int number_image=1;
	uint16_t rows=2048;
	uint16_t cols=2048;
	uint16_t indice;
	uint32_t stdimagesize=rows*cols;
	uint32_t stdCentersSize = LMAX_ROWS*LMAX_COLS;
	uint32_t stdMeanSize = 1;


	FILE *fp;
	int nkeys;
	char **header;
	printf ("Start!\n");

	/*
	 * Memory allocation
	 * Corresponds to part of copying images to SDRAM, total size of virtual RAM
	 */
	SDRAM = (int32_t*) malloc((uint32_t)16*stdimagesize*sizeof(int32_t));
	inputimg = (int*) malloc((uint32_t)stdimagesize*sizeof(int));			//Only one image

	printf("Load images in Virtual RAM!\n");

	for(unsigned int i = 0; i < number_image; i++) {
		FITS_getImage("im00.fits", inputimg, stdimagesize, &nkeys, &header);
		//FITS_getImage("circle3.fits", inputimg, stdimagesize, &nkeys, &header);
		for (int j = 0; j < stdimagesize; j++)
			SDRAM[j]=(int32_t)eve_fp_int2s32(inputimg[j], FP32_FWL );
			//SDRAM[j]=(int32_t)inputimg[j];
	}

	/*
	 * * * * * * * * *
	 * memory mapping*
	 * * * * * * * * *
	 */

	//	1.) Load image data to (virtual) SDRAM:
	uint32_t 	img01Sdram = 0;
	uint32_t 	img01Size = stdimagesize;
	uint32_t 	img01DatasetId = 1;

	uint32_t 	img02Sdram = img01Sdram + img01Size;
	uint32_t 	img02Size = stdimagesize;
	uint32_t 	img02DatasetId = 2;
	//
	uint32_t	img03Sdram = img02Sdram + img02Size;
	uint32_t	img03Size = stdimagesize;
	uint32_t	img03DatasetId = 3;

	uint32_t	img04Sdram = img03Sdram + img03Size;
	uint32_t	img04Size = stdimagesize;
	uint32_t	img04DatasetId = 4;

	uint32_t	img05Sdram = img04Sdram + img04Size;
	uint32_t	img05Size = stdimagesize;
	uint32_t	img05DatasetId = 5;

	uint32_t	img06Sdram = img05Sdram + img05Size;
	uint32_t	img06Size = stdimagesize;
	uint32_t	img06DatasetId = 6;

	uint32_t	img07Sdram = img06Sdram + img06Size;
	uint32_t	img07Size = stdimagesize;
	uint32_t	img07DatasetId = 7;

	uint32_t	img08Sdram = img07Sdram + img07Size;
	uint32_t	img08Size = stdimagesize;
	uint32_t	img08DatasetId = 8;

	uint32_t	img09Sdram = img08Sdram + img08Size;
	uint32_t	img09Size = stdimagesize;
	uint32_t	img09DatasetId = 9;

	uint32_t    centersSdram = img09Sdram + img09Size;
	uint32_t	centersSize  = stdCentersSize;
	uint32_t	centersDatasetId = 10;

	uint32_t	meanSdram = centersSdram + centersSize;
	uint32_t	meanSize  = stdMeanSize;
	uint32_t	meanDatasetId = 11;

	uint32_t	tmp1Sdram = meanSdram + meanSize;
	uint32_t	tmp1Size = stdimagesize;
	uint32_t	tmp1DatasetId = 12;

	uint32_t	tmp2Sdram = tmp1Sdram + tmp1Size;
	uint32_t	tmp2Size = stdimagesize;
	uint32_t	tmp2DatasetId = 13;

	uint32_t	tmp3Sdram = tmp2Sdram + tmp2Size;
	uint32_t	tmp3Size = stdimagesize;
	uint32_t	tmp3DatasetId = 14;

	/*
	uint32_t	med7Sdram = img6Sdram + img6Size;
	uint32_t	med7Size = 1*sizeof(int32_t);
	uint32_t	med7DatasetId = 7;

	uint32_t	rad8Sdram = med7Sdram + med7Size;
	uint32_t	rad8Size = LMAX_ROWS*LMAX_COLS*sizeof(int32_t);
	uint32_t	rad8DatasetId = 8;
*/


	img01=(SDRAM+img01Sdram);
	img02=(SDRAM+img02Sdram);
	img03=(SDRAM+img03Sdram);
	img04=(SDRAM+img04Sdram);
	img05=(SDRAM+img05Sdram);
	img06=(SDRAM+img06Sdram);
	img07=(SDRAM+img07Sdram);
	img08=(SDRAM+img08Sdram);
	img09=(SDRAM+img09Sdram);
	centers=(SDRAM+centersSdram);
	mean=(SDRAM+meanSdram);
	tmp1=(SDRAM+tmp1Sdram);
	tmp2=(SDRAM+tmp2Sdram);
	tmp3=(SDRAM+tmp3Sdram);

	preprocessing_vmem_setEntry(img01Sdram, img01Size, img01DatasetId, img01);
	preprocessing_vmem_setEntry(img02Sdram, img02Size, img02DatasetId, img02);
	preprocessing_vmem_setEntry(img03Sdram, img03Size, img03DatasetId, img03);
	preprocessing_vmem_setEntry(img04Sdram, img04Size, img04DatasetId, img04);
	preprocessing_vmem_setEntry(img05Sdram, img05Size, img05DatasetId, img05);
	preprocessing_vmem_setEntry(img06Sdram, img06Size, img06DatasetId, img06);
	preprocessing_vmem_setEntry(img07Sdram, img07Size, img07DatasetId, img07);
	preprocessing_vmem_setEntry(img08Sdram, img08Size, img08DatasetId, img08);
	preprocessing_vmem_setEntry(img09Sdram, img09Size, img09DatasetId, img09);

	preprocessing_vmem_setEntry(centersSdram, centersSize, centersDatasetId, centers);
	preprocessing_vmem_setEntry(meanSdram, meanSize, meanDatasetId, mean);

	preprocessing_vmem_setEntry(tmp1Sdram, tmp1Size, tmp1DatasetId, tmp1);
	preprocessing_vmem_setEntry(tmp2Sdram, tmp2Size, tmp2DatasetId, tmp2);
	preprocessing_vmem_setEntry(tmp3Sdram, tmp3Size, tmp3DatasetId, tmp3);

	preprocessing_vmem_print();


	//	2.) Process image data:
		printf("Pipeline  start!\n");

#if DEBUG
		fp=fopen("img.fits","wb");
		fwrite(img01,sizeof(int32_t),stdimagesize,fp);
		fclose(fp);

		preprocessing_ana_maxImage(img01Sdram,rows,cols, img03Sdram, max);
		preprocessing_ana_minImage(img01Sdram,rows,cols, img03Sdram, min);
		printf("minimo antes del suavisado  = %f\n",  eve_fp_signed32ToDouble(m, FP32_FWL));
		printf("maximo antes del suavisado = %f\n",eve_fp_signed32ToDouble(M, FP32_FWL));
#endif



#if DEBUG

		preprocessing_ana_median(img01Sdram,rows, cols,tmp1Sdram);

		preprocessing_ana_deriveX(tmp1Sdram,rows,cols,tmp2Sdram);
		preprocessing_arith_abs(tmp1Sdram,rows,cols,tmp1Sdram);

		preprocessing_ana_deriveY(tmp1Sdram,rows,cols,tmp3Sdram);
		preprocessing_arith_abs(tmp2Sdram,rows,cols,tmp2Sdram);

		preprocessing_arith_addImages(tmp2Sdram, tmp3Sdram, rows, cols, tmp1Sdram);

		fp=fopen("derivada.fits","wb");
		fwrite(tmp1,sizeof(int32_t),stdimagesize,fp);
		fclose(fp);


		preprocessing_ana_maxImage(tmp1Sdram,rows,cols, img03Sdram, max);
		preprocessing_ana_minImage(tmp1Sdram,rows,cols, img03Sdram, min);
		printf("minimo despues del suavisado  = %f\n",  eve_fp_signed32ToDouble(m, FP32_FWL));
		printf("maximo despues del suavisado = %f\n",eve_fp_signed32ToDouble(M, FP32_FWL));
#endif

		printf("Pipeline  get gradient estimation from input image 0..255!\n");
		preprocessing_ana_deriveX(img01Sdram,rows,cols,tmp1Sdram);
		preprocessing_arith_abs(tmp1Sdram,rows,cols,tmp1Sdram);

		preprocessing_ana_deriveY(img01Sdram,rows,cols,tmp2Sdram);
		preprocessing_arith_abs(tmp2Sdram,rows,cols,tmp2Sdram);

		preprocessing_arith_addImages(tmp1Sdram, tmp2Sdram, rows, cols, tmp3Sdram);


#if DEBUG
		printf("Write to File gradiente ....finished!\n");
		fp=fopen("gradiente.fits","wb");
		fwrite(tmp3,sizeof(int32_t),stdimagesize,fp);
		fclose(fp);

		preprocessing_ana_maxImage(tmp3Sdram,rows,cols, img03Sdram, max);
		preprocessing_ana_minImage(tmp3Sdram,rows,cols, img03Sdram, min);
		printf("minimo despues del suavisado  = %f\n",  eve_fp_signed32ToDouble(m, FP32_FWL));
		printf("maximo despues del suavisado = %f\n",eve_fp_signed32ToDouble(M, FP32_FWL));
#endif

		//image thresholding
		preprocessing_arith_meanImage2(tmp3Sdram, rows,cols, meanSdram);
		threshold = *mean;
		dif_threshold=eve_fp_signed32ToDouble(threshold, FP32_FWL);

		while (abs(dif_threshold)>0.5){
			preprocessing_Threshold(tmp3Sdram, rows, cols,threshold,&newThresh);
			dif_threshold=eve_fp_signed32ToDouble(eve_fp_subtract32(threshold, newThresh), FP32_FWL);
			threshold=newThresh;
		}

		printf("Threshold is ========  = %f  \n",  eve_fp_signed32ToDouble(threshold, FP32_FWL));

		printf("Thresholding................!\n");
		preprocessing_ana_over_eq_Thresh(tmp3Sdram, rows, cols,threshold, tmp3Sdram);

#if DEBUG
		//for debug
		printf("Write to binary image to File finished!\n");
		fp=fopen("imgbin2.fits","wb");
		fwrite(tmp3,sizeof(int32_t),stdimagesize,fp);
		fclose(fp);
		//for debug end
#endif


		int radio=963;
		indice=0;
		float Rmin=(float)radio-(float)WIDTH_RADIO/2;
		float Rmax=(float)radio+(float)WIDTH_RADIO/2;
		//for(float r=Rmin;r<Rmax;r+=STEP_RADIO)
		//{
			//printf("Radio: %f\n", r);
			preprocessing_zero(tmp1Sdram, rows,cols, tmp1Sdram);				//Reset Accumulator
			preprocessing_hough(img01Sdram,tmp1Sdram,rows,cols, radio, CENTER_DIST, STEP_HOUGH);
			//preprocessing_hough(tmp3Sdram,tmp1Sdram,rows,cols, r , CENTER_DIST, STEP_HOUGH);

			preprocessing_ana_median(tmp1Sdram,rows, cols,tmp2Sdram);
			preprocessing_maximumValue(tmp2Sdram, rows, cols, centersSdram, indice);

		//}

#if DEBUG
		printf("Write to acumulador image to File finished!\n");
		fp=fopen("acumula.fits","wb");
		fwrite(tmp1,sizeof(int32_t),stdimagesize,fp);
		fclose(fp);


		printf("Write to File finished!\n");


		//FITS_saveImage(outputimg, "ones2.fits", rows, cols, nkeys, &header);

#endif
		printf("Done!\n");
		return 1;

}
