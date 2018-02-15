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

/**
 * Calculates hough of an image and saves the center
 *
 * @param sdSrc1		the VMEM (SDRAM) address of source image.
 * @param sdSrc2		the VMEM (SDRAM) address of accumulator image.
 * @param rows 			the number of image rows.
 * @param cols 			the number of image columns.
 * @param centerDist	the maximum center distance to find the center of solar disc
 * @param stepSize		the value of the step used in the hough algorithm
 * @param radius   		the current radio
 * @param dst			the VMEM (SDRAM) address of result: solar disc center and votes.
 *
 * @return PREPROCESSING_SUCCESSFUL on success, failure code otherwise.
 */

int preprocessing_hough_accumulate(uint32_t sdSrc1, uint32_t sdSrc2, uint16_t rows,
        uint16_t cols, int32_t centerDist, int32_t stepSize, int32_t radius,
        int32_t *dst);
#endif /* LIBPREPROCESSING_PREPROCESSING_HOUGH_H_ */
