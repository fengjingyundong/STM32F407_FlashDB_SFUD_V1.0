#ifndef __DWT_H
#define __DWT_H

#include "main.h"

#ifdef __cplusplus
extern "C"
{
#endif

  void DWT_Init(void);            /* DWT ��ʼ�� */
  void DWT_Delay_us(uint32_t us); /* DWT ΢����ʱ */
  void DWT_Delay100us(void);      /* DWT ΢����ʱ100us */

#ifdef __cplusplus
}
#endif

#endif /* __DWT_H */
