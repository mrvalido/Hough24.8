/*
 * hough.h
 *
 *  Created on: 17 may. 2017
 *      Author: zaca
 */

#ifndef LIBPREPROCESSING_PREPROCESSING_HOUGH_H_
#define LIBPREPROCESSING_PREPROCESSING_HOUGH_H_

#include <sys/types.h>
#include <stdint.h>
#include "../FITS_Interface.h"

//NAND FLASH METHODS
void createNANDFLASH(int32_t *NANDFLASH, int32_t **entriesOfNAND, int stdimagesize, int numberOfImages);
int readNAND(int32_t *nandSrc, uint16_t rows, uint16_t cols, uint32_t sdDst);
int writeNAND(uint32_t sdSrc, uint16_t rows, uint16_t cols, int32_t *nandDst);
//END OF NAND FLASH METHODS

/**
	 * Calculate absolute of an image
	 *
	 * @param sdSrc  	the VMEM (SDRAM) address of image.
	 * @param rows   	the number of image rows.
	 * @param cols   	the number of image columns.
	 * @param sdDst  	the VMEM (SDRAM) address of result: abs of image.
	 *
	 * @return PREPROCESSING_SUCCESSFUL on success, failure code otherwise.
	 */
int preprocessing_arith_abs(uint32_t sdSrc, uint16_t rows, uint16_t cols, uint32_t sdDst);

/**
     * Calcualte hough of an image
     *
     * @param sdSrc  	the VMEM (SDRAM) address of image.
     * @param rows   	the number of image rows.
     * @param cols   	the number of image columns.
     * @param Xmin   	the minimum value of X where the center can be
     * @param Xmax   	the maximum value of X where the center can be
     * @param Ymin   	the minimum value of Y where the center can be
     * @param Ymax   	the maximum value of Y where the center can be
     * @param r2	   	the pow2 of the radio
     * @param step   	the value of the step used in the hough algorithm
     * @param sdDst  	the VMEM (SDRAM) address of result: mask of image i.
     *
     * @return PREPROCESSING_SUCCESSFUL on success, failure code otherwise.
     */
int preprocessing_hough(uint32_t sdSrc, uint16_t rows,
		uint16_t cols, unsigned int Xmin,unsigned int Xmax, unsigned int Ymin,unsigned int Ymax, float r2, float step, uint32_t sdDst);

/**
     * Fill an image with 0
     *
     * @param sdSrc  	the VMEM (SDRAM) address of image.
     * @param rows   	the number of image rows.
     * @param cols   	the number of image columns.
     * @param sdDst  	the VMEM (SDRAM) address of result.
     *
     * @return PREPROCESSING_SUCCESSFUL on success, failure code otherwise.
     */
int preprocessing_zero(uint32_t sdSrc, uint16_t rows, uint16_t cols, uint32_t sdDst);

/**
     * Update matrix of centers
     *
     * @param sdSrc  	the VMEM (SDRAM) address of image.
     * @param rows   	the number of image rows.
     * @param cols   	the number of image columns.
     * @param disp_max	the maximum dispersion
     * @param step		the step used in the hough algorithm
     * @param index  	the index of the current image.
     * @param sdDst  	the VMEM (SDRAM) address of result: matrix of centers.
     *
     * @return PREPROCESSING_SUCCESSFUL on success, failure code otherwise.
     */
int preprocessing_maximumValue(uint32_t sdSrc, uint16_t rows, uint16_t cols,
		int disp_max, float step, int16_t index, uint32_t sdDst);

/**
     * Binarize source image, in order to use it in hough
     *
     * @param sdSrc  	the VMEM (SDRAM) address of image.
     * @param sdTmp1	the VMEM (SDRAM) address of tmp1.
     * @param sdTmp2	the VMEM (SDRAM) address of tmp2
     * @param rows   	the number of image rows.
     * @param cols   	the number of image columns.
     * @param sdDst  	the VMEM (SDRAM) address of result.
     *
     * @return PREPROCESSING_SUCCESSFUL on success, failure code otherwise.
     */
int preprocessing_binarize(uint32_t sdSrc, uint32_t sdTmp1, uint32_t sdTmp2, uint16_t rows, uint16_t cols, uint32_t sdDst);

#endif /* LIBPREPROCESSING_PREPROCESSING_HOUGH_H_ */
