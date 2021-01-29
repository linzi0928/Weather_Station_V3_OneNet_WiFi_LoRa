/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND 
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, SEMTECH SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * 
 * Copyright (C) SEMTECH S.A.
 */
/*! 
 * \file       sx12xxEiger.c
 * \brief        
 *
 * \version    1.0
 * \date       Nov 21 2012
 * \author     Miguel Luis
 */
#include <stdint.h> 
#include "spi.h"
#include "sx12xxEiger.h"


#include "USART.h"


// System tick (1ms)
volatile uint32_t TickCounter = 0;

void BoardInit( void )
{
    /* Setup SysTick Timer for 1 us interrupts ( not too often to save power ) */
    if( SysTick_Config( SystemCoreClock / 1000 ) )    //1ms
    { 
        /* Capture error */ 
        while (1);
    }

    // Initialize SPI
    //SpiInit( );

}

void Delay ( uint32_t delay )
{
    // Wait delay ms
    uint32_t startTick = TickCounter;
    while( ( TickCounter - startTick ) < delay );   
}

void LongDelay ( uint8_t delay )
{
    uint32_t longDelay;
    uint32_t startTick;

    longDelay = delay * 1000;

    // Wait delay s
    startTick = TickCounter;
    while( ( TickCounter - startTick ) < longDelay );   
}
