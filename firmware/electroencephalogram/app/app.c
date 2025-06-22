/*
 * app.c
 *
 *  Created on: May 25, 2025
 *      Author: stephan
 */

#include "hw_adc.h"

void app_setup(void)
{
	hw_adc_init();
	hw_adc_start_acquisition();
}

void app_loop(void)
{

}
