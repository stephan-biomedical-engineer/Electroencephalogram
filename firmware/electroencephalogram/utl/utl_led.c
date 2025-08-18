#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "hw.h"
#include "utl_led.h"

#define UTL_LED_PROCESS_MS 100
#define UTL_LED_MAX_STATES 10
#define REGULAR_LEDS 1

typedef struct state_led_pat_ctrl_s
{
    uint8_t state;
    uint16_t counter;
    utl_led_mode_t mode;
    hw_pin_id_t pin;
    bool in_use;
} state_led_pat_ctrl_t;

typedef struct state_led_pattern_s
{
    // quantidade de tempos, sempre na ordem on / off
    uint8_t num_states;
    // tempos de ligado e desligado, em sequencia
    uint16_t states_duration_100ms[UTL_LED_MAX_STATES];
} state_led_pattern_t;
/*
const state_led_pattern_t state_led_mode_patterns[] =
{
    [UTL_LED_MODE_ON]                = { .num_states = 1, .states_duration_100ms = {1} }, // on
    [UTL_LED_MODE_OFF]               = { .num_states = 1, .states_duration_100ms = {0} }, // off
    [UTL_LED_MODE_BLINK_FAST_ONCE]   = { .num_states = 1, .states_duration_100ms = {2} }, // on once and off after
    [UTL_LED_MODE_BLINK_NORMAL_ONCE] = { .num_states = 1, .states_duration_100ms = {4} }, // on once and off after
    [UTL_LED_MODE_BLINK_SLOW_ONCE]   = { .num_states = 1, .states_duration_100ms = {8} }, // on once and off after
    [UTL_LED_MODE_BLINK_FAST_1X]     = { .num_states = 2, .states_duration_100ms = {1,14} }, // 1 piscadinhas rapidas num ciclo de 2s
    [UTL_LED_MODE_BLINK_FAST_2X]     = { .num_states = 4, .states_duration_100ms = {1,2,1,16} }, // 2 piscadinhas rapidas num ciclo de 2s
    [UTL_LED_MODE_BLINK_FAST_3X]     = { .num_states = 6, .states_duration_100ms = {1,2,1,2,1,14} }, // 3 piscadinhas rapidas num ciclo de 2s
    [UTL_LED_MODE_BLINK_FAST_4X]     = { .num_states = 8, .states_duration_100ms = {1,2,1,2,1,2,1,10} }, // 4 piscadinhas rapidas num ciclo de 2s
    [UTL_LED_MODE_BLINK_FAST_5X]     = { .num_states = 10, .states_duration_100ms = {1,2,1,2,1,2,1,2,1,8} }, // 4 piscadinhas rapidas num ciclo de 2s
};
*/
const state_led_pattern_t state_led_mode_patterns[] =
{
    [UTL_LED_MODE_ON]                  = { .num_states = 1, .states_duration_100ms = {1} }, // on
    [UTL_LED_MODE_OFF]                 = { .num_states = 1, .states_duration_100ms = {0} }, // off
    [UTL_LED_MODE_BLINK_FAST_ONCE]     = { .num_states = 1, .states_duration_100ms = {1} }, // on once and off after
    [UTL_LED_MODE_BLINK_NORMAL_ONCE]   = { .num_states = 1, .states_duration_100ms = {2} }, // on once and off after
    [UTL_LED_MODE_BLINK_SLOW_ONCE]     = { .num_states = 1, .states_duration_100ms = {4} }, // on once and off after
    [UTL_LED_MODE_BLINK_SUPER_FAST_1X] = { .num_states = 2, .states_duration_100ms = {1,3} },
    [UTL_LED_MODE_BLINK_FAST_1X]       = { .num_states = 2, .states_duration_100ms = {1,19} }, // 1 piscadinhas rapidas num ciclo de 1s
    [UTL_LED_MODE_BLINK_FAST_2X]       = { .num_states = 4, .states_duration_100ms = {1,2,1,15} }, // 2 piscadinhas rapidas num ciclo de 1s
    [UTL_LED_MODE_BLINK_FAST_3X]       = { .num_states = 6, .states_duration_100ms = {1,2,1,2,1,13}   }, // 3 piscadinhas rapidas num ciclo de 1s
    [UTL_LED_MODE_BLINK_FAST_4X]       = { .num_states = 8, .states_duration_100ms = {1,2,1,2,1,2,1,10}   }, // 4 piscadinhas rapidas num ciclo de 1s
    [UTL_LED_MODE_BLINK_NORM_1X]       = { .num_states = 2, .states_duration_100ms = {1,39} }, // 1 piscadinhas rapidas num ciclo de 2s
    [UTL_LED_MODE_BLINK_NORM_2X]       = { .num_states = 4, .states_duration_100ms = {1,2,1,36} }, // 2 piscadinhas rapidas num ciclo de 2s
    [UTL_LED_MODE_BLINK_NORM_3X]       = { .num_states = 6, .states_duration_100ms = {1,2,1,2,1,33}   }, // 3 piscadinhas rapidas num ciclo de 2s
    [UTL_LED_MODE_BLINK_SLOW_1X]       = { .num_states = 2, .states_duration_100ms = {1,59} }, // 1 piscadinhas lenta num ciclo de 4s
    [UTL_LED_MODE_BLINK_SLOW_2X]       = { .num_states = 4, .states_duration_100ms = {1,2,1,56} }, // 2 piscadinhas lenta num ciclo de 4s
};

volatile bool started = false;
state_led_pat_ctrl_t state_led_pat_ctrl[UTL_LED_NUM_AUTO_LEDS];

void hw_gpio_set(hw_pin_id_t pin, bool state)
{
	if(pin == HW_PIN_ID_LED_RED)
	{
		HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,state ? GPIO_PIN_SET: GPIO_PIN_RESET);
	}
}

utl_led_mode_t utl_led_mode_get(hw_pin_id_t pin)
{
    utl_led_mode_t mode = UTL_LED_INVALID_MODE;
    uint32_t led;

    for(led = 0 ; led < UTL_LED_NUM_AUTO_LEDS ; led++)
    {
        if((state_led_pat_ctrl[led].in_use == true) && (state_led_pat_ctrl[led].pin == pin))
        {
            mode = state_led_pat_ctrl[led].mode;
            break;
        }
    }
    return mode;
}

static void utl_led_exec(void)
{
    uint32_t led;

    for(led = 0 ; led < UTL_LED_NUM_AUTO_LEDS ; led++)
    {
        if(state_led_pat_ctrl[led].in_use == false)
            continue;

        uint8_t mode = state_led_pat_ctrl[led].mode;

        if(mode == UTL_LED_MODE_ON)
        {
        	hw_gpio_set(state_led_pat_ctrl[led].pin,true);
        }
        else if(mode == UTL_LED_MODE_OFF)
        {
        	hw_gpio_set(state_led_pat_ctrl[led].pin,false);
        }
        else
        {
            uint8_t num_states = state_led_mode_patterns[mode].num_states;
            uint16_t state_duration = state_led_mode_patterns[mode].states_duration_100ms[state_led_pat_ctrl[led].state];

            if(++state_led_pat_ctrl[led].counter >= state_duration)
            {
                if(num_states == 1)
                {
                	hw_gpio_set(state_led_pat_ctrl[led].pin,false);
                    // impede o religamento
                    state_led_pat_ctrl[led].counter = state_duration;
                }
                else
                {
                    state_led_pat_ctrl[led].counter = 0;
                    if(++state_led_pat_ctrl[led].state >= num_states)
                        state_led_pat_ctrl[led].state = 0;

                    // indices pares sao relacionados a estado ligado, impares a estado desligado
                    if(state_led_pat_ctrl[led].state & 0x01)
                    	hw_gpio_set(state_led_pat_ctrl[led].pin,false);
                    else
                    	hw_gpio_set(state_led_pat_ctrl[led].pin,true);
                }
            }
        }
    }
}

void utl_led_mode_set(hw_pin_id_t pin, utl_led_mode_t new_mode)
{
    uint32_t led;
    int32_t index = -1;

    if(new_mode >= UTL_LED_MAX_MODES)
        return;

    // already in list ?
    for(led = 0 ; led < UTL_LED_NUM_AUTO_LEDS ; led++)
    {
        if(state_led_pat_ctrl[led].in_use == true && state_led_pat_ctrl[led].pin == pin)
            break;

        // save a free position for avoiding a second search
        if(state_led_pat_ctrl[led].in_use == false)
            index = led;
    }

    // if pin is not in use, allocate a new slot
    if(led >= UTL_LED_NUM_AUTO_LEDS)
    {
        // no room ...
        if(index == -1)
            return;

        led = index;
        state_led_pat_ctrl[led].mode = UTL_LED_MODE_OFF;
        state_led_pat_ctrl[led].state = 0;
        state_led_pat_ctrl[led].counter = 0;
        state_led_pat_ctrl[led].pin = pin;
    }

    // operacao normal:
    // se o modo for diferente ou se for de piscada unica, aceita
    // na piscada unica, a contagem reinicia, efeito desejado
    // na piscada ciclica nao queremos mudar o ciclo em curso
    //
    // no entanto, para o caso de o led ser rgb, para menter o sincronismo, vai ser
    // necessario restart a contagem para gerar a cor necessaria

#ifdef REGULAR_LEDS
    if(new_mode != state_led_pat_ctrl[led].mode || state_led_mode_patterns[new_mode].num_states == 1)
    {
        state_led_pat_ctrl[led].mode = new_mode;
        state_led_pat_ctrl[led].state = 0;
        state_led_pat_ctrl[led].counter = 0;

        hw_gpio_set(state_led_pat_ctrl[led].pin,new_mode == UTL_LED_MODE_OFF);
    }
#else
    state_led_pat_ctrl[led].mode = new_mode;
    state_led_pat_ctrl[led].state = 0;
    state_led_pat_ctrl[led].counter = 0;

    hw_gpio_set(state_led_pat_ctrl[led].pin,new_mode == UTL_LED_MODE_OFF);
#endif

    state_led_pat_ctrl[led].in_use = true;
}

void utl_led_50ms_callback(void)
{
    if(started)
        utl_led_exec();
}

void utl_led_pause(void)
{
	started = false;
}

void utl_led_resume(void)
{
	started = true;
}

void utl_led_init(void)
{
    uint32_t n;

    for(n = 0 ; n < UTL_LED_NUM_AUTO_LEDS ; n++)
        state_led_pat_ctrl[n].in_use = false;

    started = true;
}


