/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#ifndef BOARDS_H
#define BOARDS_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(BOARD_NRF6310)
  #include "boards/nrf6310.h"
#elif defined(BOARD_PCA10000)
  #include "boards/pca10000.h"
#elif defined(BOARD_PCA10001)
  #include "boards/pca10001.h"
#elif defined(BOARD_PCA10003)
  #include "boards/pca10003.h"
#elif defined(BOARD_PCA1000x)
  #include "boards/pca1000x.h"
#else
#error "Board is not defined"
#endif

#ifdef __cplusplus
}
#endif

#endif
