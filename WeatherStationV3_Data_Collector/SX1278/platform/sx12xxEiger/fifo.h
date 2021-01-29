#ifndef __FIFO_H__
#define __FIFO_H__

#include <stdbool.h>

#if defined( STM32F4XX )
    #include "stm32f4xx.h"
#elif defined( STM32F2XX )
    #include "stm32f2xx.h"
#else
    #include "stm32f10x.h"
#endif

/*!
 * FIFO
 */
typedef struct sFifo
{
	uint16_t Begin;
	uint16_t End;
	uint16_t *Data;
    uint16_t Size;
}tFifo;

void FifoInit( tFifo *fifo, uint16_t *buffer, uint16_t size );

void FifoPush( tFifo *fifo, uint16_t data );

uint16_t FifoPop( tFifo *fifo );

void FifoFlush( tFifo *fifo );

bool IsFifoEmpty( tFifo *fifo );

bool IsFifoFull( tFifo *fifo );

#endif // __FIFO_H__
