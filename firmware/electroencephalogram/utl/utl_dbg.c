#include "hal.h"
#include "hw.h"
#include "utl_printf.h"

//static const char *log_modules[] =
//{
//#define X(MOD,INDEX) #MOD,
//    XMACRO_DBG_MODULES
//#undef X
//};

static uint32_t utl_dbg_modules_activated = 0;

void utl_dbg_module_enable(utl_dbg_modules_t mod_idx)
{
    utl_dbg_modules_activated |= 1 << mod_idx;
}

void utl_dbg_module_disable(utl_dbg_modules_t mod_idx)
{
    utl_dbg_modules_activated &= ~((uint32_t)(1 << mod_idx));
}

bool utl_dbg_module_is_enabled(utl_dbg_modules_t mod_idx)
{
    return (utl_dbg_modules_activated & (1 << mod_idx)) > 0;
}

void utl_dbg_printfl(utl_dbg_modules_t mod_idx, const char *fmt, ...)
{
    if(utl_dbg_module_is_enabled(mod_idx))
    {
        va_list arg;
        va_start(arg, fmt);

        utl_printf("%lu ",hw_tick_ms_get());
        utl_vprintf(fmt,arg);

        va_end(arg);
    }
}

void utl_dbg_printfsl(utl_dbg_modules_t mod_idx, const char *fmt)
{
    if(utl_dbg_module_is_enabled(mod_idx))
    {
        utl_printf("%lu ",hw_tick_ms_get());
        utl_printf("%s",fmt);
    }
}

static void utl_dbg_dump_lines(char *stamp, uint8_t *data, uint16_t size)
{
    uint8_t *ptr = data;

    utl_printf("%s",stamp);

    for(uint32_t pos = 0 ; pos < size ; pos++)
    {
        if(pos && (pos % 32 == 0))
            utl_printf("\n%s",stamp);

        if(pos % 32 == 0)
            utl_printf("%04X ",(unsigned int)pos);

        utl_printf("%02X",*ptr++);
    }
    utl_printf("\n");
}

void utl_dbg_dump(utl_dbg_modules_t mod_idx, uint8_t *data, uint32_t size)
{
    if(utl_dbg_module_is_enabled(mod_idx))
    {
        char stamp[64] = { 0 };

        utl_printf("%lu ",hw_tick_ms_get());
        utl_dbg_dump_lines(stamp,data,size);
    }
}

void utl_dbg_init(void)
{
    utl_dbg_module_enable(UTL_DBG_LEVEL_APP);
    //utl_dbg_module_enable(UTL_DBG_LEVEL_SER);
   // utl_dbg_module_enable(UTL_DBG_LEVEL_FLASH);
    //utl_dbg_module_enable(UTL_DBG_LEVEL_PORT);
    //utl_dbg_module_enable(UTL_DBG_LEVEL_CFG);
    //utl_dbg_module_enable(UTL_DBG_LEVEL_RTC);
    //utl_dbg_module_enable(UTL_DBG_LEVEL_ZGB);
    //utl_dbg_module_enable(UTL_DBG_LEVEL_LOG);
    //utl_dbg_module_enable(UTL_DBG_LEVEL_GPIO);
    //utl_dbg_module_enable(UTL_DBG_LEVEL_CMD);
    //utl_dbg_module_enable(UTL_DBG_LEVEL_CMD);
//    utl_dbg_module_enable(UTL_DBG_LEVEL_MHB);
}
