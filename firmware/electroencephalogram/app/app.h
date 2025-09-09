/*
 * app.h
 *
 *  Created on: May 25, 2025
 *      Author: stephan
 */

#ifndef APP_H_
#define APP_H_

void app_setup(void);
void app_loop(void);
void process_adc_chunk(uint16_t *buf, int offset, int len);

#endif /* APP_H_ */
