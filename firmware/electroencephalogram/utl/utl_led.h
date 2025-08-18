/**

@file
@defgroup LED LED
@brief Funções para uso automatizado de LEDs.
@{

*/
#pragma once

/**
@brief Modos possíveis de controle de LEDs.
*/
typedef enum utl_led_mode_s
{
    UTL_LED_MODE_OFF = 0,           /**< LED desligado */
    UTL_LED_MODE_ON,                /**< LED ligado */
    UTL_LED_MODE_BLINK_FAST_ONCE,   /**< LED pisca uma vez somente, com tempo curto quando ligado */
    UTL_LED_MODE_BLINK_NORMAL_ONCE, /**< LED pisca uma vez somente, com tempo normal quando ligado */
    UTL_LED_MODE_BLINK_SLOW_ONCE,   /**< LED pisca uma vez somente, com tempo longo quando ligado */
    UTL_LED_MODE_BLINK_SUPER_FAST_1X,
    UTL_LED_MODE_BLINK_FAST_1X,     /**< LED piscando uma vez, continuamente e rápido */
    UTL_LED_MODE_BLINK_FAST_2X,     /**< LED piscando duas vez, continuamente e rápido */
    UTL_LED_MODE_BLINK_FAST_3X,     /**< LED piscando três vez, continuamente e rápido */
	UTL_LED_MODE_BLINK_FAST_4X,
	UTL_LED_MODE_BLINK_NORM_1X,
	UTL_LED_MODE_BLINK_NORM_2X,
	UTL_LED_MODE_BLINK_NORM_3X,
    UTL_LED_MODE_BLINK_SLOW_1X,     /**< LED piscando uma vez, continuamente e lento */
    UTL_LED_MODE_BLINK_SLOW_2X,     /**< LED piscando duas vez, continuamente e lento */
    UTL_LED_MAX_MODES,               /**< Número máximo de modos */
    UTL_LED_INVALID_MODE = -1
} utl_led_mode_t;

#define UTL_LED_NUM_AUTO_LEDS 1

typedef enum hw_pin_id_e
{
	HW_PIN_ID_LED_RED = 0,
} hw_pin_id_t;

/**
@brief Configura o modo de funcionamento de um LED ou pino.
@param[in] pin - pino a ser configurado (ver @ref ulora_gpio_pin_e)
@param[in] mode - modo de operação (ver @ref utl_led_mode_t)

@code

// pisca 3 vezes, rápido e de forma contínua

utl_led_mode_set(UTL_HW_GPIO_PIN_LED0,UTL_LED_MODE_BLINK_FAST_3X);

@endcode
*/
void utl_led_mode_set(hw_pin_id_t pin, utl_led_mode_t mode);
utl_led_mode_t utl_led_mode_get(hw_pin_id_t pin);
void utl_led_resume(void);
void utl_led_pause(void);

#ifndef DOXYGEN_SKIP_INTERNAL_FUNCTIONS
void utl_led_50ms_callback(void); /**< Led processing callback */
void utl_led_init(void); /**< Initialize led processing */
#endif

/**
@}
*/
