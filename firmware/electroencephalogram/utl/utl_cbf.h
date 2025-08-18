
#pragma once

typedef enum utl_cbf_status_s
{
    UTL_CBF_OK = 0, 
    UTL_CBF_FULL,   
    UTL_CBF_EMPTY,  
    UTL_CBF_TMROUT,
} utl_cbf_status_t;

typedef struct utl_cbf_s
{
    volatile size_t prod; 
    volatile size_t cons; 
    uint16_t size;         
    uint8_t *buffer;       
} utl_cbf_t;

#define UTL_CBF_DECLARE(name,_size)        \
    static uint8_t name##buffer[_size + 1];     \
    static utl_cbf_t name = {              \
        .prod = 0,                              \
        .cons = 0,                              \
        .size = _size+1,                        \
        .buffer = (uint8_t*) name##buffer,      \
    }

/**
 @brief Retorna a quantidade de bytes disponível para consumo num buffer circular.
 @param[in] cb - ponteiro para o buffer circular.
 @return quantidade de bytes disponível para consumo
*/
uint16_t utl_cbf_bytes_available(utl_cbf_t *cb);
/**
 @brief Esvazia um buffer circular.
 @param[in] cb - ponteiro para o buffer circular.
 @return ver @ref cbf_status_s
*/
utl_cbf_status_t utl_cbf_flush(utl_cbf_t *cb);
/**
 @brief Retira um byte do buffer circular.
 @param[in] cb - ponteiro para o buffer circular.
 @param[out] c - ponteiro para o destino do dado (previamente alocado).
 @return ver @ref cbf_status_s
*/
utl_cbf_status_t utl_cbf_get(utl_cbf_t *cb, uint8_t *c);
/**
 @brief Reinicializa um buffer circular, caso seja necessário.
 A macro @ref CBF_DECLARE já faz esse papel mas essa função pode ser usada para inicialização de forma
 independente da macro.
 @param[in] cb - ponteiro para o buffer circular.
 @param[in] area - buffer previamente alocado que será usado para armazenamento do conteúdo do buffer circular.
 @param[in] size - tamanho da área de dados apontada por @p area.
 @return ver @ref cbf_status_s
*/
utl_cbf_status_t utl_cbf_init(utl_cbf_t *cb, uint8_t *area, uint16_t size);
/**
 @brief Coloca um byte no buffer circular.
 @param[in] cb - ponteiro para o buffer circular.
 @param[in] c - byte a ser adicionado ao buffer circular.
 @return ver @ref cbf_status_s
*/
utl_cbf_status_t utl_cbf_put(utl_cbf_t *cb, uint8_t c);

