


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
	int32_t *img1;
	int32_t *img2;
	int32_t *img3;
	int32_t *img4;
	int32_t *img5;
	int32_t *img6;
	int32_t *med7;
	int32_t *rad8;

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
	uint32_t histsize=256;

	FILE *fp;
	int nkeys;
	char **header;
	printf ("Start!\n");

	/*
	 * Memory allocation
	 * Corresponds to part of copying images to SDRAM, total size of virtual RAM
	 */
	SDRAM = (int32_t*) malloc((uint32_t)32*stdimagesize*sizeof(int32_t));
	inputimg = (int*) malloc((uint32_t)stdimagesize*sizeof(int));//solo una imagen


	printf("load images in Virtual RAM*****!\n");
	//
	for(unsigned int i = 0; i < number_image; i++) {
		//imageName[8] = 48 + i;//for "./im/im0X.fits" , "./hr/im0X.fits"; set
		FITS_getImage("im00.fits", inputimg, stdimagesize, &nkeys, &header);
		for (int j = 0; j < stdimagesize; j++)
			SDRAM[j]=(int32_t)eve_fp_int2s32(inputimg[j], FP32_FWL );
	}

	/*
	 * * * * * * * * *
	 *    memory mapping   *
	 * * * * * * * * *
	 */

	//	1.) Load image data to (virtual) SDRAM:
	uint32_t 	img1Sdram = 0;
	uint32_t 	img1Size = stdimagesize*sizeof(int32_t);
	uint32_t 	img1DatasetId = 1;

	uint32_t 	img2Sdram = img1Sdram + img1Size;
	uint32_t 	img2Size = stdimagesize*sizeof(int32_t);
	uint32_t 	img2DatasetId = 2;
	//
	uint32_t	img3Sdram = img2Sdram + img2Size;
	uint32_t	img3Size = stdimagesize*sizeof(int32_t);
	uint32_t	img3DatasetId = 3;

	uint32_t	img4Sdram = img3Sdram + img3Size;
	uint32_t	img4Size = stdimagesize*sizeof(int32_t);
	uint32_t	img4DatasetId = 4;

	uint32_t	img5Sdram = img4Sdram + img4Size;
	uint32_t	img5Size = stdimagesize*sizeof(int32_t);
	uint32_t	img5DatasetId = 5;

	uint32_t	img6Sdram = img5Sdram + img5Size;//for store HIST
	uint32_t	img6Size = histsize*sizeof(int32_t);
	uint32_t	img6DatasetId = 6;

	uint32_t	med7Sdram = img6Sdram + img6Size;//for store HIST
	uint32_t	med7Size = 1*sizeof(int32_t);
	uint32_t	med7DatasetId = 7;

	uint32_t	rad8Sdram = med7Sdram + med7Size;//for store HIST
	uint32_t	rad8Size = LMAX_ROWS*LMAX_COLS*sizeof(int32_t);
	uint32_t	rad8DatasetId = 8;



	img1=(SDRAM+img1Sdram);
	img2=(SDRAM+img2Sdram);
	img3=(SDRAM+img3Sdram);
	img4=(SDRAM+img4Sdram);
	img5=(SDRAM+img5Sdram);
	img6=(SDRAM+img6Sdram);
	med7=(SDRAM+med7Sdram);
	rad8=(SDRAM+rad8Sdram);


	preprocessing_vmem_setEntry(img1Sdram, img1Size, img1DatasetId, img1);
	preprocessing_vmem_setEntry(img2Sdram, img2Size, img2DatasetId, img2);
	preprocessing_vmem_setEntry(img3Sdram, img3Size, img3DatasetId, img3);
	preprocessing_vmem_setEntry(img4Sdram, img4Size, img4DatasetId, img4);
	preprocessing_vmem_setEntry(img5Sdram, img5Size, img5DatasetId, img5);
	preprocessing_vmem_setEntry(img6Sdram, img6Size, img6DatasetId, img6);
	preprocessing_vmem_setEntry(med7Sdram, med7Size, med7DatasetId, med7);
	preprocessing_vmem_setEntry(rad8Sdram, rad8Size, rad8DatasetId, rad8);
	preprocessing_vmem_print();


	//	2.) Process image data:
		printf("Pipeline  start!\n");

#if DEBUG
		fp=fopen("img.fits","wb");
		fwrite(img1,sizeof(int32_t),stdimagesize,fp);
		fclose(fp);

		preprocessing_ana_maxImage(img1Sdram,rows,cols, img4Sdram, max);
		preprocessing_ana_minImage(img1Sdram,rows,cols, img5Sdram, min);
		printf("minimo antes del suavisado  = %f\n",  eve_fp_signed32ToDouble(m, FP32_FWL));
		printf("maximo antes del suavisado = %f\n",eve_fp_signed32ToDouble(M, FP32_FWL));
#endif
		preprocessing_ana_median(img1Sdram,rows, cols,img2Sdram);
#if DEBUG
		preprocessing_ana_maxImage(img2Sdram,rows,cols, img4Sdram, max);
		preprocessing_ana_minImage(img2Sdram,rows,cols, img5Sdram, min);
		printf("minimo despues del suavisado  = %f\n",  eve_fp_signed32ToDouble(m, FP32_FWL));
		printf("maximo despues del suavisado = %f\n",eve_fp_signed32ToDouble(M, FP32_FWL));
#endif
		//preprocessing_arith_divideScalar(img2Sdram,rows, cols,2304,img2Sdram);

#if DEBUG
		//for debug
		/*		printf("Pipeline  Scaling input image 0..255!\n");
		preprocessing_ana_maxImage(img2Sdram,rows,cols, img4Sdram, max);
		preprocessing_ana_minImage(img2Sdram,rows,cols, img5Sdram, min);
		printf("minimo  = %f\n",  eve_fp_signed32ToDouble(m, FP32_FWL));
		printf("maximo = %f\n",eve_fp_signed32ToDouble(M, FP32_FWL));
		preprocessing_arith_subtractScalar(img2Sdram,rows, cols,m,img2Sdram);
		preprocessing_arith_divideScalar(img2Sdram,rows, cols,M,img2Sdram);
		preprocessing_arith_multiplyScalar(img2Sdram,rows, cols,Scale,img2Sdram);


		preprocessing_ana_maxImage(img2Sdram,rows,cols, img4Sdram, max);
		preprocessing_ana_minImage(img2Sdram,rows,cols, img5Sdram, min);
		printf("Scale  = %f\n",  eve_fp_signed32ToDouble(Scale, FP32_FWL));
		printf("minimo  = %f\n",  eve_fp_signed32ToDouble(m, FP32_FWL));
		printf("maximo = %f\n",eve_fp_signed32ToDouble(M, FP32_FWL));
		fp=fopen("img255.fits","wb");
		fwrite(img1,sizeof(int32_t),stdimagesize,fp);
		fclose(fp);*/
		//for debug end

#endif


		printf("Pipeline  get gradient estimation from input image 0..255!\n");
		preprocessing_ana_deriveX(img1Sdram,rows,cols,img3Sdram);
		preprocessing_arith_abs(img3Sdram,rows,cols,img3Sdram);

		preprocessing_ana_deriveY(img1Sdram,rows,cols,img4Sdram);
		preprocessing_arith_abs(img4Sdram,rows,cols,img4Sdram);

		preprocessing_arith_addImages(img3Sdram, img4Sdram, rows, cols, img2Sdram);

#if DEBUG
		printf("Write to File gradiente ....finished!\n");
		fp=fopen("gradiente.fits","wb");
		fwrite(img2,sizeof(int32_t),stdimagesize,fp);
		fclose(fp);
#endif

		//image thresholding
		preprocessing_arith_meanImage2(img2Sdram, rows,cols, med7Sdram);
		threshold=*med7;
		dif_threshold=eve_fp_signed32ToDouble(threshold, FP32_FWL);

		while (abs(dif_threshold)>0.5){
			preprocessing_Threshold(img2Sdram, rows, cols,threshold,&newThresh);
			dif_threshold=eve_fp_signed32ToDouble(eve_fp_subtract32(threshold, newThresh), FP32_FWL);
			threshold=newThresh;
		}

		printf("Threshold is ========  = %f  \n",  eve_fp_signed32ToDouble(threshold, FP32_FWL));

		printf("Thresholding................!\n");
		preprocessing_ana_over_eq_Thresh(img2Sdram, rows, cols,threshold, img2Sdram);

#if DEBUG
		//for debug
		printf("Write to binary image to File finished!\n");
		fp=fopen("imgbin2.fits","wb");
		fwrite(img3,sizeof(int32_t),stdimagesize,fp);
		fclose(fp);
		//for debug end
#endif


		int radio=963;
		indice=0;
		float Rmin=(float)radio-(float)WIDTH_RADIO/2;
		float Rmax=(float)radio+(float)WIDTH_RADIO/2;
		for(float r=Rmin;r<Rmax;r+=STEP_RADIO)
		{

			preprocessing_zero(img3Sdram, rows,cols, img3Sdram);				//Reset Accumulator
			preprocessing_hough(img2Sdram,img3Sdram,rows,cols, r , CENTER_DIST, STEP_HOUGH);
			preprocessing_ana_median(img3Sdram,rows, cols,img4Sdram);
			preprocessing_maximumValue(img4Sdram, rows, cols, rad8Sdram, indice);

		}

#if DEBUG
		printf("Write to acumulador image to File finished!\n");
		fp=fopen("acumula.fits","wb");
		fwrite(img2,sizeof(int32_t),stdimagesize,fp);
		fclose(fp);


		printf("Write to File finished!\n");


		//FITS_saveImage(outputimg, "ones2.fits", rows, cols, nkeys, &header);

#endif
		printf("Done!\n");
		return 1;

}
