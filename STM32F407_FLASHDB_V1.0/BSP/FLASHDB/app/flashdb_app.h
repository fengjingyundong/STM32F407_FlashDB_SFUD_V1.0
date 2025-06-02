#ifndef __FLASHDB_APP_H
#define __FLASHDB_APP_H
#include "main.h"
#include <sfud.h>

#ifdef __cplusplus
extern "C"
{
#endif

  extern sfud_flash sfud_norflash0;

  int spi_flash_init(void); /* FlashDB ≥ı ºªØ */

  int flashdb_app_Start(void); /* FlashDB_App_Start FlashDB≤‚ ‘demo≥Ã–Ú */

#ifdef __cplusplus
}
#endif

#endif /* __FLASHDB_H */
