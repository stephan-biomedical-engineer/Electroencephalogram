/*
 * hw_adc.c
 *
 *  Created on: Jun 14, 2025
 *      Author: stephan
 */

#include <stdint.h>
#include "main.h"
#include "app.h"
#include "hw_adc.h"
#include "stm32h7xx_hal_adc.h"
#include "stm32h7xx_hal_adc_ex.h"

#define EEG_SAMPLE_SIZE     (EEG_NUM_CHANNELS * 2)  // 2 bytes por canal (12 bits ADC)
#define EEG_PACKET_HEADER   4                   // Timestamp (4 bytes)
#define EEG_PACKET_SIZE     (EEG_PACKET_HEADER + EEG_SAMPLE_SIZE)

// --- Variáveis Globais (definidas aqui, declaradas extern em hw_adc.h) ---
extern ADC_HandleTypeDef hadc1; // Assume que hadc1 é globalmente definido (geralmente pelo CubeMX)
extern TIM_HandleTypeDef htim3;

// Buffer para armazenar os dados do ADC. Declará-lo como uint16_t é mais eficiente
// já que o RightBitShift_6 garantirá que os dados caibam em 16 bits.
uint16_t adc_buffer[ADC_DMA_BUFFER_SIZE_SAMPLES];

// Flag para indicar que novos dados estão disponíveis (usar com cuidado, RTOS seria melhor)
// Esta flag será setada dentro dos callbacks do DMA.
volatile uint8_t adc_data_ready = 0; // Usar 'volatile' pois é modificada por uma ISR
uint8_t adc_samples[EEG_SAMPLE_SIZE];

// Implementação interna da configuração do ADC
static void hw_adc_1_config(void) // Renomeada
{
  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};


  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_16B;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = EEG_NUM_CHANNELS;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc1.Init.OversamplingMode = ENABLE;
  hadc1.Init.Oversampling.Ratio = 64;
  hadc1.Init.Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_6;
  hadc1.Init.Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
  hadc1.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;


  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  // Configuração dos canais (todos com o mesmo SamplingTime)
  sConfig.SamplingTime = ADC_SAMPLETIME_32CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;

  // Canal 1
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  // Canal 2
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  // Canal 3
  sConfig.Channel = ADC_CHANNEL_16;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  // Canal 4
  sConfig.Channel = ADC_CHANNEL_17;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

// Implementação da configuração do TIM3
//static void hw_tim_3_config(void)
//{
//  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
//  TIM_MasterConfigTypeDef sMasterConfig = {0};
//  htim3.Instance = TIM3;
//  htim3.Init.Prescaler = 30-1;
//  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
//  htim3.Init.Period = 250-1;
//  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
//
//  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
//  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//}


// --- Implementação das Funções de API (Públicas) ---

/**
  * @brief  Inicializa e configura o periférico ADC para aquisição de EEG.
  * Esta função deve ser chamada apenas UMA VEZ na inicialização do sistema.
  * @retval None
  */
void hw_adc_init(void)
{
  hw_adc_1_config(); // Chama a função de configuração interna
//  hw_tim_3_config();
}

/**
  * @brief  Inicia a aquisição contínua de dados do ADC via DMA.
  * A função hw_adc_init() deve ter sido chamada previamente.
  * @retval None
  */
void hw_adc_start_acquisition(void)
{
  // Inicia a transferência DMA em modo circular.
  // O buffer é uint16_t, mas HAL_ADC_Start_DMA espera uint32_t*.
  // O typecast é seguro porque ADC_RIGHTBITSHIFT_6 garante que o dado de 16 bits
  // estará corretamente alinhado e sem truncamento no uint16_t do buffer.
  hw_adc_init();

//  if(HAL_TIM_Base_Start(&htim3) != HAL_OK)
//  {
//    Error_Handler();
//  }

  if(HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED) != HAL_OK)
  {
	Error_Handler();
  }

  HAL_Delay(100);

  if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_DMA_BUFFER_SIZE_SAMPLES) != HAL_OK)
  {
    Error_Handler(); // Tratar erro se o DMA não iniciar
  }
}

/**
  * @brief  Para a aquisição de dados do ADC via DMA.
  * @retval None
  */
void hw_adc_stop_acquisition(void)
{
  if (HAL_ADC_Stop_DMA(&hadc1) != HAL_OK)
  {
    Error_Handler(); // Tratar erro se o DMA não parar
  }
}


// --- Implementação das Funções Estáticas (Internas do Módulo) ---

// Exemplo: buffer de 1024 amostras, 4 canais
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
    process_adc_chunk(adc_buffer, 0, ADC_DMA_BUFFER_SIZE_SAMPLES / 2);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    process_adc_chunk(adc_buffer, ADC_DMA_BUFFER_SIZE_SAMPLES / 2,
                      ADC_DMA_BUFFER_SIZE_SAMPLES / 2);
}


/**
  * @brief  Callback de erro do ADC.
  * @param  hadc Ponteiro para o handle do ADC.
  * @retval None
  */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
  if(hadc->Instance == ADC1)
  {
    // Um erro de ADC ou DMA ocorreu (ex: overrun do buffer).
    // Implemente sua lógica de tratamento de erros aqui:
    // 1. Pare a aquisição para evitar mais erros: hw_adc_stop_acquisition();
    // 2. Sinalize um estado de erro global para sua aplicação.
    // 3. (Opcional) Logue o erro ou pisque um LED de erro.
    Error_Handler(); // Requer que Error_Handler esteja definido e trate apropriadamente.
  }
}
