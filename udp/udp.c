/*
 * udp.c
 *
 *  Created on: 02 feburary. 2018
 *      Author: dennis
 */

#include "udp.h"



/* PUBLIC IMPLEMENTATION *****************************************************/

void udp_createNANDFLASH(int32_t *NANDFLASH, int32_t **entriesOfNAND,
		int stdimagesize, int numberOfImages){

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

/*****************************************************************************/

int udp_loadImage(int32_t *nandSrc, uint16_t rows, uint16_t cols,
        uint32_t sdDst)
{
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

/*****************************************************************************/

int udp_storeImage(uint32_t sdSrc, uint16_t rows, uint16_t cols,
        int32_t *nandDst)
{
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

/*****************************************************************************/

int udp_abs(uint32_t sdSrc, uint16_t rows, uint16_t cols, uint32_t sdDst)
{
    int status = PREPROCESSING_SUCCESSFUL;

    CHECK_STATUS(preprocessing_ana_underThresh(sdSrc, rows, cols, 0, sdDst))
    CHECK_STATUS(preprocessing_arith_multiplyScalar(sdDst, rows, cols, -512, sdDst))
    CHECK_STATUS(preprocessing_arith_addScalar(sdDst, rows, cols, 256, sdDst))
    CHECK_STATUS(preprocessing_arith_multiplyImages(sdSrc, sdDst, rows, cols, sdDst))

    return status;
}


/*****************************************************************************/
int udp_abs_new(uint32_t sdSrc, uint16_t rows, uint16_t cols, uint32_t sdDst)
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


/*****************************************************************************/

int udp_binarize(uint32_t sdSrc, uint32_t sdTmp1, uint32_t sdTmp2,
        uint16_t rows, uint16_t cols, uint32_t sdDst)
{
	int status = PREPROCESSING_SUCCESSFUL;

	//Calculate DX
	CHECK_STATUS(preprocessing_ana_underThresh(sdTmp1, rows, cols, EVE_FP32_NAN, sdTmp1))
	CHECK_STATUS(preprocessing_ana_deriveX(sdSrc, rows, cols, sdTmp1))
	//CHECK_STATUS(udp_abs(sdTmp1, rows, cols, sdTmp1))
	CHECK_STATUS(udp_abs_new(sdTmp1,rows, cols,sdTmp1) )


	//Calculate DY
	CHECK_STATUS(preprocessing_ana_underThresh(sdTmp2, rows, cols, EVE_FP32_NAN, sdTmp2))
	CHECK_STATUS(preprocessing_ana_deriveY(sdSrc, rows, cols,sdTmp2))
	//CHECK_STATUS(udp_abs(sdTmp2, rows, cols, sdTmp2))
	CHECK_STATUS(udp_abs_new(sdTmp2,ROWS, COLS,sdTmp2) )


	//Calculate Sum DX & DY
	CHECK_STATUS(preprocessing_arith_addImages(sdTmp1, sdTmp2, rows, cols, sdTmp2))

	//Calculate border
	CHECK_STATUS(preprocessing_arith_subtractImages(sdTmp2, sdSrc, rows, cols, sdTmp2))
	CHECK_STATUS(preprocessing_ana_overThresh(sdTmp2, rows, cols, 0, sdDst))

	return status;
}

