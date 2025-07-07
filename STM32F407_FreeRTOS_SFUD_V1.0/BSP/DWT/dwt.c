#include "dwt.h"


/* DWT ��ʼ�� */
void DWT_Init(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; /* ���� Trace ��Ԫ */
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;            /* ����DWT CYCCNT ������ */
  DWT->CYCCNT = 0;                                /* ��������� */
}

/* DWT ΢����ʱ */
void DWT_Delay_us(uint32_t us)
{
  uint32_t startTick = DWT->CYCCNT;
  uint32_t delayTicks = us * (SystemCoreClock / 1000000);
  while (DWT->CYCCNT - startTick < delayTicks)
    ;
}

/* DWT ΢����ʱ100us */
void DWT_Delay100us(void)
{
  DWT_Delay_us(100);
}
