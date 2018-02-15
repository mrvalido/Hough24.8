/*
 * udp.h
 *
 *  Created on: 02 feburary. 2018
 *      Author: dennis
 */

#ifndef UDP_UDP_H
#define UDP_UDP_H

#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* from libeve */
#include "../libeve/eve/fixed_point.h"

/* from libpreprocessing */
#include "../libpreprocessing/preprocessing/ana.h"
#include "../libpreprocessing/preprocessing/arith.h"
#include "../libpreprocessing/preprocessing/def.h"
#include "../libpreprocessing/preprocessing/def_hough.h"
#include "../libpreprocessing/preprocessing/vmem.h"

/* from fits */
#include "../fits/FITS_Interface.h"


/**
 * NAND creation, reading and writing functions.
 *
 * @return PREPROCESSING_SUCCESSFUL on success, failure code otherwise.
 * @{
 */
void udp_createNANDFLASH(int32_t *NANDFLASH, int32_t **entriesOfNAND,
        int stdimagesize,  int numberOfImages);
int udp_loadImage(int32_t *nandSrc, uint16_t rows, uint16_t cols,
            uint32_t sdDst);
int udp_storeImage(uint32_t sdSrc, uint16_t rows, uint16_t cols,
        int32_t *nandDst);
/**
 * @}
 */

/**
 * Calculate absolute values of an image
 *
 * @param sdSrc the VMEM (SDRAM) address of image.
 * @param rows  the number of image rows.
 * @param cols  the number of image columns.
 * @param sdDst the VMEM (SDRAM) address of result: abs of image.
 *
 * @return PREPROCESSING_SUCCESSFUL on success, failure code otherwise.
 */
int udp_abs(uint32_t sdSrc, uint16_t rows, uint16_t cols, uint32_t sdDst);

/**
 * Calculate absolute values of an image
 *
 * @param sdSrc the VMEM (SDRAM) address of image.
 * @param rows  the number of image rows.
 * @param cols  the number of image columns.
 * @param sdDst the VMEM (SDRAM) address of result: abs of image.
 *
 * @return PREPROCESSING_SUCCESSFUL on success, failure code otherwise.
 */
int udp_abs_new(uint32_t sdSrc, uint16_t rows, uint16_t cols, uint32_t sdDst);

/**
 * Binarize source image, in order to use it in hough
 *
 * @param sdSrc  the VMEM (SDRAM) address of image.
 * @param sdTmp1 the VMEM (SDRAM) address of tmp1.
 * @param sdTmp2 the VMEM (SDRAM) address of tmp2
 * @param rows   the number of image rows.
 * @param cols   the number of image columns.
 * @param sdDst  the VMEM (SDRAM) address of result.
 *
 * @return PREPROCESSING_SUCCESSFUL on success, failure code otherwise.
 */
int udp_binarize(uint32_t sdSrc, uint32_t sdTmp1, uint32_t sdTmp2,
        uint16_t rows, uint16_t cols, uint32_t sdDst);

#endif /* UDP_UDP_H */
