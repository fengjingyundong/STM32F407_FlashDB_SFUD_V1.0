#ifndef __FLASHDB_APP_H
#define __FLASHDB_APP_H
#include "main.h"
#include <sfud.h>
#include "string.h"

#ifdef __cplusplus
extern "C"
{
#endif

  extern sfud_flash sfud_norflash0;

  typedef struct
  {
    char str[20];
    uint16_t count1;
    uint16_t count2;
    uint16_t count3;
    uint16_t count4;
    uint16_t count5;
    uint16_t count6;
    uint16_t count7;
    uint16_t count8;
    uint16_t count9;
    uint16_t count10;
  } boot_count_t;

  int spi_flash_init(void); /* FlashDB ≥ı ºªØ */

  int flashdb_app_Start(void); /* FlashDB_App_Start FlashDB≤‚ ‘demo≥Ã–Ú */

#ifdef __cplusplus
}
#endif

#endif /* __FLASHDB_H */
