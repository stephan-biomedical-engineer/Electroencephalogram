#include "led.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "esp_attr.h"

#define LED_GPIO 2  // Definido fixamente aqui

static const char *TAG = "LED";
static bool s_led_state = false;
static gptimer_handle_t s_timer_handle = NULL;

static bool IRAM_ATTR timer_isr_callback(gptimer_handle_t timer, 
                                        const gptimer_alarm_event_data_t *edata, 
                                        void *user_data)
{
    s_led_state = !s_led_state;
    gpio_set_level(LED_GPIO, s_led_state);
    return false;
}

void led_init(void)
{
    // Configuração fixa do GPIO
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    // Configuração do timer
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &s_timer_handle));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_isr_callback,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(s_timer_handle, &cbs, NULL));
    ESP_ERROR_CHECK(gptimer_enable(s_timer_handle));
    
}

void led_set_frequency(float freq_hz)
{
    if (freq_hz <= 0) {
        ESP_LOGE(TAG, "Frequência deve ser positiva");
        return;
    }

    // Para o timer se estiver em execução
    esp_err_t err = gptimer_stop(s_timer_handle);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_ERROR_CHECK(err);
    }

    uint64_t alarm_period = (uint64_t)(1000000.0f / (2 * freq_hz));
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = alarm_period,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(s_timer_handle, &alarm_config));
    
    // Inicia o timer
    err = gptimer_start(s_timer_handle);
    if (err == ESP_ERR_INVALID_STATE) {
        // Timer já iniciado, não precisa fazer nada
    } else {
        ESP_ERROR_CHECK(err);
    }
    
    ESP_LOGI(TAG, "Frequência setada para %.2f Hz", freq_hz);
}
