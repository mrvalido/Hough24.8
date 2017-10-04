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

//NAND FLASH METHODS
void createNANDFLASH(int32_t *NANDFLASH, int32_t **entriesOfNAND, int stdimagesize, int numberOfImages);
int readNAND(int32_t *nandSrc, uint16_t rows, uint16_t cols, uint32_t sdDst);
int writeNAND(uint32_t *sdSrc, uint16_t rows, uint16_t cols, int32_t nandDst);
//END OF NAND FLASH METHODS

int preprocessing_hough_hist(uint32_t sdSrc, uint16_t rows, uint16_t cols,
        unsigned int valor, uint32_t sdDst);
int preprocessing_arith_abs(uint32_t sdSrc, uint16_t rows, uint16_t cols, uint32_t sdDst);
int preprocessing_arith_meanImage2(uint32_t sdSrc, uint16_t rows, uint16_t cols,uint32_t sdDst);
int preprocessing_arith_sumImage2(uint32_t sdSrc, uint16_t rows, uint16_t cols,uint32_t sdDst);
int preprocessing_Threshold(uint32_t sdSrc, uint16_t rows, uint16_t cols,int32_t thresh,int32_t *newThresh);
int preprocessing_ana_over_eq_Thresh(uint32_t sdSrc, uint16_t rows, uint16_t cols,
        int32_t thresh, uint32_t sdDst);
int preprocessing_hough(int32_t sdSrc,uint32_t sdDst, uint16_t rows,
		uint16_t cols, unsigned int radio , int disp_max, float step);
int preprocessing_do_hough(const int32_t *src, int32_t *dst, uint16_t rows,
		uint16_t cols, unsigned int Xmin,unsigned int Xmax, float r2, float step);
int preprocessing_zero(uint32_t sdSrc, uint16_t rows, uint16_t cols, uint32_t sdDst);
int preprocessing_maximumValue(uint32_t sdSrc, uint16_t rows, uint16_t cols,
        uint32_t sdDst, int16_t index);
#endif /* LIBPREPROCESSING_PREPROCESSING_HOUGH_H_ */
