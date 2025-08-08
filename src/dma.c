//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMA.C #################################
//---------------------------------------------

#include "dma.h"
#include "stm32f10x.h"

#include "adc.h"


// Module Private Types Constants and Macros -----------------------------------
#define RCC_DMA1_CLK    (RCC->AHBENR & RCC_AHBENR_DMA1EN)
#define RCC_DMA1_CLK_ON    (RCC->AHBENR |= RCC_AHBENR_DMA1EN)
#define RCC_DMA1_CLK_OFF    (RCC->AHBENR &= ~RCC_AHBENR_DMA1EN)


/* Externals variables ---------------------------------------------------------*/
extern volatile unsigned short adc_ch [];

/* Global variables ---------------------------------------------------------*/


/* Module Definitions ---------------------------------------------------------*/


/* Module functions ---------------------------------------------------------*/
void DMAConfig(void)
{
    /* DMA1 clock enable */
    if (!RCC_DMA1_CLK)
        RCC_DMA1_CLK_ON;

    //Configuro el control del DMA CH1
    DMA1_Channel1->CCR = 0;
    //priority very high
    //memory halfword
    //peripheral halfword
    //increment memory
    DMA1_Channel1->CCR |= DMA_CCR1_PL | DMA_CCR1_MSIZE_0 | DMA_CCR1_PSIZE_0 | DMA_CCR1_MINC;
    //DMA1_Channel1->CCR |= DMA_Mode_Circular | DMA_CCR_TCIE;
    //cicular mode
    DMA1_Channel1->CCR |= DMA_CCR1_CIRC;

    //Tamaño del buffer a transmitir
    DMA1_Channel1->CNDTR = ADC_CHANNEL_QUANTITY;

    //Address del periferico
    DMA1_Channel1->CPAR = (uint32_t) &ADC1->DR;

    //Address en memoria
    DMA1_Channel1->CMAR = (uint32_t) &adc_ch[0];

    //Enable
    //DMA1_Channel1->CCR |= DMA_CCR_EN;
#ifdef DMA_WITH_INTERRUPT
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    NVIC_SetPriority(DMA1_Channel1_IRQn, 5);
#endif
}

void DMAEnableInterrupt (void)
{
    DMA1_Channel1->CCR |= DMA_CCR1_TCIE;
}

void DMADisableInterrupt (void)
{
    DMA1_Channel1->CCR &= ~DMA_CCR1_TCIE;
}

#ifdef DMA_WITH_INTERRUPT
void DMA1_Channel1_IRQHandler (void)
{
    if (sequence_ready)
    {
        // Clear DMA TC flag
        sequence_ready_reset;
        
    }
}
#endif


#include "neopixel.h"
extern unsigned char pwm_ccr2[];
void DMA1_Channel3_Config (void)
{
    // DMA1 clock enable
    if (!RCC_DMA1_CLK)
        RCC_DMA1_CLK_ON;

    //Configuro el control del DMA UP para TIM3
    DMA1_Channel3->CCR = 0;
    // priority very high
    // memory byte
    // peripheral halfword
    // increment memory
    // memory to peripheral
    DMA1_Channel3->CCR |= DMA_CCR1_PL_1 |
        DMA_CCR1_PL_0 |
        DMA_CCR1_PSIZE_0 |
        DMA_CCR1_MINC |
        DMA_CCR1_DIR;

    //Tamaño del buffer a transmitir
    DMA1_Channel3->CNDTR = SIZEOF_PIXEL_DATA;

    //Address del periferico
    DMA1_Channel3->CPAR = (uint32_t) &TIM3->CCR2;

    //Address en memoria
    DMA1_Channel3->CMAR = (uint32_t) &pwm_ccr2[0];

    //Enable
    //DMA1_Channel3->CCR |= DMA_CCR1_EN;
}

//---- end of file ----//
