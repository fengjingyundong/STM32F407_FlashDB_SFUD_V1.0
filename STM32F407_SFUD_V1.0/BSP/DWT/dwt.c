#include "dwt.h"


/* DWT 初始化 */
void DWT_Init(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; /* 启用 Trace 单元 */
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;            /* 启用DWT CYCCNT 计数器 */
  DWT->CYCCNT = 0;                                /* 清除计数器 */
}

/* DWT 微秒延时 */
void DWT_Delay_us(uint32_t us)
{
  uint32_t startTick = DWT->CYCCNT;
  uint32_t delayTicks = us * (SystemCoreClock / 1000000);
  while (DWT->CYCCNT - startTick < delayTicks)
    ;
}

/* DWT 微秒延时100us */
void DWT_Delay100us(void)
{
  DWT_Delay_us(100);
}
