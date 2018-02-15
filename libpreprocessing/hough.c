/*
 * hough.c
 *
 *  Created on: 16 may. 2017
 *      Author: zaca
 */

#include "preprocessing/hough.h"

/* from stdc */
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* from libeve */
#include "../libeve/eve/fixed_point.h"

/* from libpreprocessing */
#include "preprocessing/ana.h"
#include "preprocessing/def.h"
#include "preprocessing/def_hough.h"
#include "preprocessing/vmem.h"
#include "preprocessing/ana.h"
#include "preprocessing/arith.h"


#include <time.h>

/* PUBLIC IMPLEMENTATION *****************************************************/
int preprocessing_hough_accumulate(uint32_t sdSrc1, uint32_t sdSrc2, uint16_t rows,
        uint16_t cols, int32_t centerDist, int32_t stepSize, int32_t radius,
        int32_t *dst)
{

	int status = PREPROCESSING_SUCCESSFUL;
	unsigned int size = (unsigned int)(rows) * cols;
	unsigned int p = 0;
	unsigned int p1=0;//vector index in output image(hough space)
	float det, det1;
	float b;
	int bb, aa;
	int32_t one=FP32_BINARY_TRUE;

    unsigned int Xmin= rows / 2U - eve_fp_signed32ToDouble(centerDist, FP32_FWL); 	// Xmin and Xmax define a square boundaries of coordinates around of CCD center (XC,YC)
	unsigned int Xmax= rows / 2U + eve_fp_signed32ToDouble(centerDist, FP32_FWL);
	unsigned int Ymin= cols / 2U - eve_fp_signed32ToDouble(centerDist, FP32_FWL); 	// Xmin and Xmax define a square boundaries of coordinates around of CCD center (XC,YC)
	unsigned int Ymax= cols / 2U + eve_fp_signed32ToDouble(centerDist, FP32_FWL);

    float step = (float)(eve_fp_signed32ToDouble(stepSize, FP32_FWL));

    // Calculate power of 2 of given radius.
    float r2 = (float)(eve_fp_signed32ToDouble(radius, FP32_FWL));
    r2 *= r2;

    const int32_t* src1 = preprocessing_vmem_getDataAddress(sdSrc1);
	int32_t* src2 = preprocessing_vmem_getDataAddress(sdSrc2);

	// Check whether given rows and columns are in a valid range.
	if ((!preprocessing_vmem_isProcessingSizeValid(sdSrc1, rows, cols))
			|| (!preprocessing_vmem_isProcessingSizeValid(sdSrc2, rows, cols)))
	{
		return PREPROCESSING_INVALID_SIZE;
	}

    // Check whether center distance exceeds image dimensions.
    if ((eve_fp_signed32ToDouble(centerDist, FP32_FWL) > (rows / 2U)) || (eve_fp_signed32ToDouble(centerDist, FP32_FWL) > (cols / 2U)))
    {
        //TRACED_PRINT("Center distance %u exceeds image dimensions (%u,%u)", centerDist, rows, cols);
        printf("Center distance %u exceeds image dimensions (%u,%u)",
                        centerDist, rows, cols);
        return PREPROCESSING_INVALID_SIZE;
    }

	//Reset Accumulator
    preprocessing_ana_underThresh(sdSrc2, rows, cols, EVE_FP32_NAN, sdSrc2);

	for (unsigned int r = 0; r < rows; r++)
	{
		for (unsigned int c = 0; c < cols; c++)
		{
			p = r * cols + c;

			// Check for valid pointer position.
			PREPROCESSING_DEF_CHECK_POINTER(src1, p, size);

			if (eve_fp_compare32(src1 + p, &one) == 0){//check for data equal to one
				//process
				for(float a=(float)(Xmin); a<(float)(Xmax);a+=step){
					det=r2-((float)(c)-a)*((float)(c)-a);//det to loop over xc and compute yc
					det1=r2-((float)(r)-a)*((float)(r)-a);//de1t to loop over yc and compute xc

					//yc estimation from xc loop
					if (det>0){
						b=(float)((float)(r)-sqrt(det));//yc
						if (b>(float)(Ymin) && b<(float)(Ymax)){//check for yc €[Xmin,Xmax]
							aa=(int)round((a-(float)(Xmin))/step);//xc
							bb=(int)round((b-(float)(Ymin))/step);//yc
							if (bb>0 && aa>0){
								p1=(unsigned int)(bb*cols + aa);
								PREPROCESSING_DEF_CHECK_POINTER(src2, p1, size)
								src2[p1] = eve_fp_add32(src2[p1], FP32_BINARY_TRUE);
								if (src2[p1] == EVE_FP32_NAN)
								{
									status = PREPROCESSING_INVALID_NUMBER;
								}
								//If current pixel count is better than the max, change it
								if(eve_fp_compare32(dst, src2+p1) == -1){
									dst[0] = src2[p1];
									dst[1] = aa*(int32_t)(step) + (int32_t)(Xmin);
									dst[2] = bb*(int32_t)(step) + (int32_t)(Ymin);
								}
							}
						}
					}
					//xc1 estimation from yc1 loop
					if (det1>0){
						b=(float)((float)(c)-sqrt(det1));//xc1
						if (b>Xmin && b<Xmax){ //check for xc€[Xmin,Xmax]
							aa=(int)round((a-(float)(Ymin))/step);//yc
							bb=(int)round((b-(float)(Xmin))/step);//xc
							if (bb>0 && aa>0){
								p1=(unsigned int)(aa*cols + bb);
								PREPROCESSING_DEF_CHECK_POINTER(src2, p1, size)
								src2[p1] = eve_fp_add32(src2[p1], FP32_BINARY_TRUE);
								if (src2[p1] == EVE_FP32_NAN)
								{
									status = PREPROCESSING_INVALID_NUMBER;
								}
								//If current pixel count is better than the max, change it
								if(eve_fp_compare32(dst, src2+p1) == -1){
									dst[0] = src2[p1];
									dst[1] = bb*(int32_t)(step) + (int32_t)(Xmin);
									dst[2] = aa*(int32_t)(step) + (int32_t)(Ymin);
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
