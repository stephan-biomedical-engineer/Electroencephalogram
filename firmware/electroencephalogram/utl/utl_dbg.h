#pragma once

#define UTL_DBG                1
#define UTL_DBG_USING_SEGGER   0

#define XMACRO_DBG_MODULES \
    X(UTL_DBG_LEVEL_APP,0) \
    X(UTL_DBG_LEVEL_FLASH,1) \
    X(UTL_DBG_LEVEL_ACCEL,2) \
    X(UTL_DBG_LEVEL_SER,3) \
    X(UTL_DBG_LEVEL_EVT,4) \
    X(UTL_DBG_LEVEL_PORT,5) \
    X(UTL_DBG_LEVEL_CFG,6) \
    X(UTL_DBG_LEVEL_RTC,7) \
    X(UTL_DBG_LEVEL_ZGB,8) \
    X(UTL_DBG_LEVEL_LOG,9) \
	X(UTL_DBG_LEVEL_TMR,10) \
	X(UTL_DBG_LEVEL_GPIO,11) \
    X(UTL_DBG_LEVEL_CMD,12) \
    X(UTL_DBG_LEVEL_MHB,13)

typedef enum utl_dbg_modules_e
{
#define X(MOD,INDEX) MOD = INDEX,
    XMACRO_DBG_MODULES
#undef X
} utl_dbg_modules_t;

void utl_dbg_printfl(utl_dbg_modules_t mod_idx, const char *fmt, ...);
void utl_dbg_printfsl(utl_dbg_modules_t mod_idx, const char *fmt);

#define UTL_DBG_PRINTFL(level,fmt, ...) utl_dbg_printfl(level,(const char *)fmt,__VA_ARGS__)
#define UTL_DBG_PRINTFSL(level,fmt)     utl_dbg_printfsl(level,(const char *)fmt)

void utl_dbg_init(void);
void utl_dbg_module_enable(utl_dbg_modules_t mod_idx);
void utl_dbg_module_disable(utl_dbg_modules_t mod_idx);
bool utl_dbg_module_is_enabled(utl_dbg_modules_t mod_idx);
void utl_dbg_printf(utl_dbg_modules_t mod_idx, const char *fmt, ...);
void utl_dbg_dump(utl_dbg_modules_t mod_idx, uint8_t *data, uint32_t size);
