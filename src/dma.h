//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMA.H #################################
//---------------------------------------------

#ifndef _DMA_H_
#define _DMA_H_

//--- Defines for configuration ----------------
#include "stm32f10x.h"

// #define DMA_WITH_INTERRUPT

//--- Exported Macros ---//
#define sequence_ready         (DMA1->ISR & DMA_ISR_TCIF1)
#define sequence_ready_reset   (DMA1->IFCR = DMA_ISR_TCIF1)

#define DMA_ENABLE    (DMA1_Channel1->CCR |= DMA_CCR1_EN)

#define DMA1_CH3_ENABLE    (DMA1_Channel3->CCR |= DMA_CCR1_EN)
#define DMA1_CH3_DISABLE    (DMA1_Channel3->CCR &= ~DMA_CCR1_EN)

//--- Exported constants ---//


//--- Module Functions ---//
void DMAConfig(void);
void DMAEnableInterrupt (void);
void DMADisableInterrupt (void);
void DMA1_Channel1_IRQHandler (void);

void DMA1_Channel3_Config (void);



#endif /* _DMA_H_ */
