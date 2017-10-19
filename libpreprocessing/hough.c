/*
 * hough.c
 *
 *  Created on: 16 may. 2017
 *      Author: zaca
 */


#include "preprocessing/ana.h"

#include "preprocessing/def.h"
#include "preprocessing/mydef.h"
#include "preprocessing/vmem.h"
#include "preprocessing/hough.h"
#include "preprocessing/ana.h"
#include "preprocessing/arith.h"
/* from libeve */
#include "../libeve/eve/fixed_point.h"

/* from std c */
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//NAND FLASH METHODS
void createNANDFLASH(int32_t *NANDFLASH, int32_t **entriesOfNAND, int stdimagesize, int numberOfImages){

	int nkeys;
	char **header;
	int *inputimg = (int*) malloc((uint32_t)stdimagesize*sizeof(int));			//Only one image

	printf("Loading images in NAND FLASH\n");
	char fileName[12] = "im/im00.fits";
	for(unsigned int i = 0; i < numberOfImages; i++) {
		fileName[6] = 48 + i;
		FITS_getImage(fileName, inputimg, stdimagesize, &nkeys, &header);
		for (int j = 0; j < stdimagesize; j++)
			NANDFLASH[i*stdimagesize + j]=(int32_t)eve_fp_int2s32(inputimg[j], FP32_FWL );
	}

	//	1.) Load image data to NAND Flash:
	uint32_t 	img1Nand = 0;
	uint32_t 	img2Nand = img1Nand + stdimagesize;
	uint32_t	img3Nand = img2Nand + stdimagesize;
	uint32_t	img4Nand = img3Nand + stdimagesize;
	uint32_t	img5Nand = img4Nand + stdimagesize;
	uint32_t	img6Nand = img5Nand + stdimagesize;
	uint32_t	img7Nand = img6Nand + stdimagesize;
	uint32_t	img8Nand = img7Nand + stdimagesize;
	uint32_t	img9Nand = img8Nand + stdimagesize;

	entriesOfNAND[0]=(NANDFLASH+img1Nand);
	entriesOfNAND[1]=(NANDFLASH+img2Nand);
	entriesOfNAND[2]=(NANDFLASH+img3Nand);
	entriesOfNAND[3]=(NANDFLASH+img4Nand);
	entriesOfNAND[4]=(NANDFLASH+img5Nand);
	entriesOfNAND[5]=(NANDFLASH+img6Nand);
	entriesOfNAND[6]=(NANDFLASH+img7Nand);
	entriesOfNAND[7]=(NANDFLASH+img8Nand);
	entriesOfNAND[8]=(NANDFLASH+img9Nand);
	printf("Images loaded successfully\n");
}

int readNAND(int32_t *nandSrc, uint16_t rows, uint16_t cols, uint32_t sdDst){
	int status = PREPROCESSING_SUCCESSFUL;
	unsigned int size = (unsigned int)(rows) * cols;
	unsigned int p = 0;

	int32_t* dst = preprocessing_vmem_getDataAddress(sdDst);

	// Check whether given rows and columns are in a valid range.
	if (!preprocessing_vmem_isProcessingSizeValid(sdDst, rows, cols)){
		return PREPROCESSING_INVALID_SIZE;
	}

	// Process.
	for (unsigned int r = 0; r < rows; r++)
	{
		for (unsigned int c = 0; c < cols; c++)
		{
			p = r * cols + c;

			// Check for valid pointer position.
			PREPROCESSING_DEF_CHECK_POINTER(dst, p, size);

			dst[p] = nandSrc[p];

			if (dst[p] == EVE_FP32_NAN)
			{
				status = PREPROCESSING_INVALID_NUMBER;
			}
		}
	}

	return status;
}

int writeNAND(uint32_t sdSrc, uint16_t rows, uint16_t cols, int32_t *nandDst){
	int status = PREPROCESSING_SUCCESSFUL;
	unsigned int size = (unsigned int)(rows) * cols;
	unsigned int p = 0;

	const int32_t* src = preprocessing_vmem_getDataAddress(sdSrc);

	// Check whether given rows and columns are in a valid range.
	if (!preprocessing_vmem_isProcessingSizeValid(sdSrc, rows, cols)){
		return PREPROCESSING_INVALID_SIZE;
	}

	// Process.
	for (unsigned int r = 0; r < rows; r++)
	{
		for (unsigned int c = 0; c < cols; c++)
		{
			p = r * cols + c;

			// Check for valid pointer position.
			PREPROCESSING_DEF_CHECK_POINTER(src, p, size);

			nandDst[p]=src[p];

			if (src[p] == EVE_FP32_NAN)
			{
				status = PREPROCESSING_INVALID_NUMBER;
			}
		}
	}

	return status;
}
//END OF NAND FLASH METHODS


int preprocessing_arith_abs(uint32_t sdSrc, uint16_t rows, uint16_t cols, uint32_t sdDst)
{
    int status = PREPROCESSING_SUCCESSFUL;
    unsigned int size = (unsigned int)(rows) * cols;
    unsigned int p = 0;
    uint32_t zero = 0;

    const int32_t* src = preprocessing_vmem_getDataAddress(sdSrc);
    int32_t* dst = preprocessing_vmem_getDataAddress(sdDst);

    // Check whether given rows and columns are in a valid range.
    if ((!preprocessing_vmem_isProcessingSizeValid(sdSrc, rows, cols))
            || (!preprocessing_vmem_isProcessingSizeValid(sdDst, rows, cols)))
    {
        return PREPROCESSING_INVALID_SIZE;
    }

    // Process.
    for (unsigned int r = 0; r < rows; r++)
    {
        for (unsigned int c = 0; c < cols; c++)
        {

            p = r * cols + c;

            // Check for valid pointer position.
            PREPROCESSING_DEF_CHECK_POINTER(src, p, size);
            PREPROCESSING_DEF_CHECK_POINTER(dst, p, size);


			if (eve_fp_compare32(src + p, &zero) == -1)
				dst[p]=eve_fp_multiply32(src[p],-256,FP32_FWL);		//If negative multiply by -1(-256) in 24.8 format
			else
				dst[p]=src[p];

            if (dst[p] == EVE_FP32_NAN)
            {
                status = PREPROCESSING_INVALID_NUMBER;
            }
        }
    }

    return status;
}

int preprocessing_hough(uint32_t sdSrc, uint16_t rows,
		uint16_t cols, unsigned int Xmin,unsigned int Xmax, unsigned int Ymin,unsigned int Ymax, float r2, float step, uint32_t sdDst)
{

	int status = PREPROCESSING_SUCCESSFUL;
	unsigned int size = (unsigned int)(rows) * cols;
	unsigned int p = 0;
	unsigned int p1=0;//vector index in output image(hough space)
	float det, det1;
	float b;
	int bb, aa;
	int32_t one=FP32_BINARY_TRUE;


	const int32_t* src = preprocessing_vmem_getDataAddress(sdSrc);
	int32_t* dst = preprocessing_vmem_getDataAddress(sdDst);

	// Check whether given rows and columns are in a valid range.
	if ((!preprocessing_vmem_isProcessingSizeValid(sdSrc, rows, cols))
			|| (!preprocessing_vmem_isProcessingSizeValid(sdDst, rows, cols)))
	{
		return PREPROCESSING_INVALID_SIZE;
	}

	for (unsigned int r = 0; r < rows; r++)
	{
		for (unsigned int c = 0; c < cols; c++)
		{
			p = r * cols + c;

			// Check for valid pointer position.
			PREPROCESSING_DEF_CHECK_POINTER(src, p, size);

			if (eve_fp_compare32(src + p, &one) == 0){//check for data equal to one
				//process
				for(float a=Xmin; a<Xmax;a+=step){
					det=r2-(c-a)*(c-a);//det to loop over xc and compute yc
					det1=r2-(r-a)*(r-a);//de1t to loop over yc and compute xc

					//yc estimation from xc loop
					if (det>0){
						b=((float)r-sqrt(det));//yc
						if (b>Ymin && b<Ymax){//check for yc €[Xmin,Xmax]
							aa=(int)round((a-Xmin)/step);//xc
							bb=(int)round((b-Ymin)/step);//yc
							if (bb>0 && aa>0){
								p1=bb*cols + aa;
								PREPROCESSING_DEF_CHECK_POINTER(dst, p1, size)
								dst[p1] = eve_fp_add32(dst[p1], FP32_BINARY_TRUE);
								if (dst[p1] == EVE_FP32_NAN)
								{
									status = PREPROCESSING_INVALID_NUMBER;
								}
							}
						}
					}
					//xc1 estimation from yc1 loop
					if (det1>0){
						b=((float)c-sqrt(det1));//xc1
						if (b>Xmin && b<Xmax){ //check for xc€[Xmin,Xmax]
							aa=(int)round((a-Ymin)/step);//yc
							bb=(int)round((b-Xmin)/step);//xc
							if (bb>0 && aa>0){
								p1=aa*cols + bb;
								PREPROCESSING_DEF_CHECK_POINTER(dst, p1, size)
								dst[p1] = eve_fp_add32(dst[p1], FP32_BINARY_TRUE);
								if (dst[p1] == EVE_FP32_NAN)
								{
									status = PREPROCESSING_INVALID_NUMBER;
								}
							}
						}
					}
				}
			}
		}
	}
	return status;
}

int preprocessing_zero(uint32_t sdSrc, uint16_t rows, uint16_t cols, uint32_t sdDst)
{
    int status = PREPROCESSING_SUCCESSFUL;
    unsigned int size = (unsigned int)(rows) * cols;
    unsigned int p = 0;

    const int32_t* src = preprocessing_vmem_getDataAddress(sdSrc);
    int32_t* dst = preprocessing_vmem_getDataAddress(sdDst);

    // Check whether given rows and columns are in a valid range.
    if ((!preprocessing_vmem_isProcessingSizeValid(sdSrc, rows, cols))
            || (!preprocessing_vmem_isProcessingSizeValid(sdDst, rows, cols)))
    {
        return PREPROCESSING_INVALID_SIZE;
    }

    // Process.
    for (unsigned int r = 0; r < rows; r++)
    {
        for (unsigned int c = 0; c < cols; c++)
        {
            p = r * cols + c;

            // Check for valid pointer position.
            PREPROCESSING_DEF_CHECK_POINTER(src, p, size)
            PREPROCESSING_DEF_CHECK_POINTER(dst, p, size)

				dst[p]=0;

            if (dst[p] == EVE_FP32_NAN)
            {
                status = PREPROCESSING_INVALID_NUMBER;
            }
        }
    }

    return status;
}

int preprocessing_maximumValue(uint32_t sdSrc, uint16_t rows, uint16_t cols,
		int disp_max, float step, int16_t index, uint32_t sdDst)
{
	int status = PREPROCESSING_SUCCESSFUL;
    int32_t max = EVE_FP32_NAN;
    unsigned int size = (unsigned int)(rows) * cols;
    unsigned int p = 0;
    unsigned int p2 = 0;
    int32_t maxX;
    int32_t maxY;
    unsigned int Xmin=XC-disp_max;
    unsigned int Ymin=YC-disp_max;

    const int32_t* src = preprocessing_vmem_getDataAddress(sdSrc);
    int32_t* dst = preprocessing_vmem_getDataAddress(sdDst);

    // Check whether given rows and columns are in a valid range.
    if ((!preprocessing_vmem_isProcessingSizeValid(sdSrc, rows, cols))
            || (!preprocessing_vmem_isProcessingSizeValid(sdDst, CENTERS_ROWS, CENTERS_COLS)))
    {
        return PREPROCESSING_INVALID_SIZE;
    }

    // Process.
    for (unsigned int r = 0; r < rows; r++)
    {
        for (unsigned int c = 0; c < cols; c++)
        {
            p = r * cols + c;

            // Check for valid pointer position.
            PREPROCESSING_DEF_CHECK_POINTER(src, p, size);

            if (eve_fp_compare32(src + p, &max) == 1)
            {
                max = src[p];
                maxX = (int32_t)c;
                maxY = (int32_t)r;
            }
        }
    }

    p2=index*CENTERS_COLS;

    PREPROCESSING_DEF_CHECK_POINTER(dst, p2, size);
    PREPROCESSING_DEF_CHECK_POINTER(dst, p2+1, size);
    PREPROCESSING_DEF_CHECK_POINTER(dst, p2+2, size);

    //If the new max is better than the previous, change it
    if(eve_fp_compare32(dst + p2, &max) == -1){
    	dst[p2] = max;
    	dst[p2+1] = maxX*step + Xmin;
    	dst[p2+2] = maxY*step + Ymin;
    }

    if (dst[p2] == EVE_FP32_NAN || dst[p2+1] == EVE_FP32_NAN || dst[p2+2] == EVE_FP32_NAN)
	{
		status = PREPROCESSING_INVALID_NUMBER;
	}

    return status;
}

int preprocessing_binarize(uint32_t sdSrc, uint32_t sdTmp1, uint32_t sdTmp2, uint16_t rows, uint16_t cols, uint32_t sdDst){
	int status = PREPROCESSING_SUCCESSFUL;

	//Calculate DX
	if((status = preprocessing_zero(sdTmp1, ROWS, COLS, sdTmp1) ) != PREPROCESSING_SUCCESSFUL){ printf("Status Error\n");  return status;}
	if((status = preprocessing_ana_deriveX(sdSrc,ROWS, COLS,sdTmp1) ) != PREPROCESSING_SUCCESSFUL){ printf("Status Error\n");  return status;}
	if((status = preprocessing_arith_abs(sdTmp1,ROWS, COLS,sdTmp1) ) != PREPROCESSING_SUCCESSFUL){ printf("Status Error\n");  return status;}

	//Calculate DY
	if((status = preprocessing_zero(sdTmp2, ROWS, COLS, sdTmp2) ) != PREPROCESSING_SUCCESSFUL){ printf("Status Error\n");  return status;}
	if((status = preprocessing_ana_deriveY(sdSrc,ROWS, COLS,sdTmp2) ) != PREPROCESSING_SUCCESSFUL){ printf("Status Error\n");  return status;}
	if((status = preprocessing_arith_abs(sdTmp2,ROWS, COLS,sdTmp2) ) != PREPROCESSING_SUCCESSFUL){ printf("Status Error\n");  return status;}

	//Calculate Sum DX & DY
	if((status = preprocessing_arith_addImages(sdTmp1, sdTmp2, ROWS, COLS, sdTmp2) ) != PREPROCESSING_SUCCESSFUL){ printf("Status Error\n");  return status;}

	//Calculate border
	if((status = preprocessing_arith_subtractImages(sdTmp2, sdSrc, ROWS, COLS, sdTmp2) ) != PREPROCESSING_SUCCESSFUL){ printf("Status Error\n");  return status;}
	if((status = preprocessing_ana_overThresh(sdTmp2, ROWS, COLS, 0, sdDst) ) != PREPROCESSING_SUCCESSFUL){ printf("Status Error\n");  return status;}

	return status;
}
