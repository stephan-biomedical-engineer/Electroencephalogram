/*
 * hw_adc.h
 *
 *  Created on: Jun 14, 2025
 *      Author: stephan
 */

#ifndef HW_ADC_H_
#define HW_ADC_H_

// --- Definições Globais ---
#define EEG_NUM_CHANNELS        4
#define EEG_SAMPLE_FREQ_HZ      500 // Frequência efetiva por canal pós-oversampling
                                   // (para documentação, o cálculo exato dependerá de fADC e SamplingTime)

// Tamanho do buffer DMA em número total de amostras de 16 bits.
// Deve ser um múltiplo de EEG_NUM_CHANNELS e grande o suficiente para
// usar eficientemente os callbacks HalfCplt e Cplt.
// Ex: 64 conjuntos de 4 canais = 256 amostras de 16 bits.
#define ADC_DMA_BUFFER_SIZE_SAMPLES (64 * EEG_NUM_CHANNELS) // 256

// --- Funções de API (Públicas) ---

/**
  * @brief  Inicializa e configura o periférico ADC para aquisição de EEG.
  * Esta função deve ser chamada apenas UMA VEZ na inicialização do sistema.
  * @retval None
  */
void hw_adc_init(void);

/**
  * @brief  Inicia a aquisição contínua de dados do ADC via DMA.
  * A função hw_adc_init() deve ter sido chamada previamente.
  * @retval None
  */
void hw_adc_start_acquisition(void);

/**
  * @brief  Para a aquisição de dados do ADC via DMA.
  * @retval None
  */
void hw_adc_stop_acquisition(void);

#endif /* HW_ADC_H_ */
