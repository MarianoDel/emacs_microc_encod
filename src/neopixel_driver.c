//------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### NEOPIXEL_DRIVER.C ########################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "neopixel_driver.h"
#include "dma.h"
#include "tim.h"
#include "stm32f10x.h"


// Module Private Types Constants & Macros -------------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void Neo_Driver_Send_Pixel_Data (void)
{
    TIM3_CH2_DMA_DISABLE;
    DMA1_CH3_DISABLE;        
    DMA1_Channel3_Config();
    DMA1_CH3_ENABLE;
    TIM3_CH2_DMA_ENABLE;
}

//--- end of file ---//
