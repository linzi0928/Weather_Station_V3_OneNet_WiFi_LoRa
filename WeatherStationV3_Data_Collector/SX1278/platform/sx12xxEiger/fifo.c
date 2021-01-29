
#include "fifo.h"

static uint16_t FifoNext( tFifo *fifo, uint16_t index )
{
	return ( index + 1 ) % fifo->Size;
}

void FifoInit( tFifo *fifo, uint16_t *buffer, uint16_t size )
{
	fifo->Begin = 0;
	fifo->End = 0;
	fifo->Data = buffer;
	fifo->Size = size;
}

void FifoPush( tFifo *fifo, uint16_t data )
{
	fifo->End = FifoNext( fifo, fifo->End );
	fifo->Data[fifo->End] = data;
}

uint16_t FifoPop( tFifo *fifo )
{
	uint16_t data = fifo->Data[FifoNext( fifo, fifo->Begin )];

	fifo->Begin = FifoNext( fifo, fifo->Begin );
	return data;
}

void FifoFlush( tFifo *fifo )
{
	fifo->Begin = 0;
	fifo->End = 0;
}

bool IsFifoEmpty( tFifo *fifo )
{
	return ( fifo->Begin == fifo->End );
}

bool IsFifoFull( tFifo *fifo )
{
	return ( FifoNext( fifo, fifo->End ) == fifo->Begin );
}
