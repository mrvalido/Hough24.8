/*
 * mydef.h
 *
 *  Created on: 13 jul. 2017
 *      Author: zaca
 */

#ifndef LIBPREPROCESSING_PREPROCESSING_MYDEF_H_
#define LIBPREPROCESSING_PREPROCESSING_MYDEF_H_

#define XC 1024 // center coordinate of CCD
#define YC 1024 // center coordinate of CCD

#define ROWS 2048
#define COLS 2048

#define NUMBER_OF_IMAGES 9

#define RADIO 963.0

#define WIDTH_RADIO 5 //ancho del aro del circulo en pixels

#define NUMBER_ENTRIES_SDRAM 16

#define STEP_HOUGH 1
#define CENTER_DIST 400
#define CENTERS_ROWS NUMBER_OF_IMAGES/STEP_HOUGH //Number of ternas
#define CENTERS_COLS 3


#endif /* LIBPREPROCESSING_PREPROCESSING_MYDEF_H_ */
