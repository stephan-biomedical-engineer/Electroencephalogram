/*
 * hal.h
 *
 *  Created on: Aug 15, 2025
 *      Author: stephan
 */

#pragma once

#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdalign.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include <stdatomic.h>

#include "utl.h"
#include "cbf.h"
#include "hal_ser.h"


#if defined(__GNUC__)
#define __WEAK __attribute__((weak))
#define __UNUSED __attribute__((unused))
#else
#define __WEAK
#define __UNUSED
#endif

extern hal_ser_driver_t HAL_SER_DRIVER;

void hal_init(void);
void hal_deinit(void);
