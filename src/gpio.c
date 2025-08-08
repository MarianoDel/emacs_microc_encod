//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### GPIO.C ################################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "stm32f10x.h"
#include "gpio.h"
#include "hard.h"


// Module Private Types Constants and Macros -----------------------------------
#ifdef HARDWARE_VERSION_2_0
// #define USE_EXTI_LINES
#endif

#ifdef HARDWARE_VERSION_1_0
#define USE_EXTI_LINES
#endif

#define RCC_GPIOA_CLK    (RCC->APB2ENR & 0x00000004)
#define RCC_GPIOA_CLKEN    (RCC->APB2ENR |= 0x00000004)
#define RCC_GPIOA_CLKDIS    (RCC->APB2ENR &= ~0x00000004)

#define RCC_GPIOB_CLK    (RCC->APB2ENR & 0x00000008)
#define RCC_GPIOB_CLKEN    (RCC->APB2ENR |= 0x00000008)
#define RCC_GPIOB_CLKDIS    (RCC->APB2ENR &= ~0x00000008)

#define RCC_GPIOC_CLK    (RCC->APB2ENR & 0x00000010)
#define RCC_GPIOC_CLKEN    (RCC->APB2ENR |= 0x00000010)
#define RCC_GPIOC_CLKDIS    (RCC->APB2ENR &= ~0x00000010)

#define RCC_GPIOD_CLK    (RCC->APB2ENR & 0x00000020)
#define RCC_GPIOD_CLKEN    (RCC->APB2ENR |= 0x00000020)
#define RCC_GPIOD_CLKDIS    (RCC->APB2ENR &= ~0x00000020)

#define RCC_AFIO_CLK    (RCC->APB2ENR & 0x00000001)
#define RCC_AFIO_CLKEN    (RCC->APB2ENR |= 0x00000001)
#define RCC_AFIO_CLKDIS    (RCC->APB2ENR &= ~0x00000001)


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Functions ------------------------------------------------------------
//------- GPIO REGISTERS ----------//
//
//	GPIOx->CRL	pin 7 - 0
//	3  2  1  0
//	CNF0  MODE0
//	GPIOx->CRH	pin 15 - 8
//
//      En hexa me queda cada nibble es igual a la configuracion de ese pin
//      nibble0 configura completo pin0
//
//	MODEx 00 Input (reset)
//	MODEx 01 Output 10MHz
//	MODEx 10 Output 2MHz
//	MODEx 11 Output 50MHz
//
//	Input Mode
//      las funciones alternativas de los pines estan directamente conectadad al
//      periferico en el modo input
//      CNFx 00 Analog
//      CNFx 01 Input Floating (reset)
//      CNFx 10 Input (pull up / dwn)
//      CNFx 11 Reserved
//
//	Output Mode
//      CNFx 00 Push Pull
//      CNFx 01 Open Drain
//      CNFx 10 Alternate func Push Pull
//      CNFx 11 Alternate func Open Drain
//
//      Pull-Up Pull-Dwn si el pin es Input Pup Pdwn el registro ODR coloca pull-up (1) o pull-dwn (0)
//      GPIOx->ODR 0xNNNN, 1 bit por pin
//

//-- GPIO Configuration --------------------
void GpioInit (void)
{
    unsigned long temp;

    //--- GPIO Clocks ---//
    if (!RCC_GPIOA_CLK)
        RCC_GPIOA_CLKEN;

    if (!RCC_GPIOB_CLK)
        RCC_GPIOB_CLKEN;

    if (!RCC_GPIOC_CLK)
        RCC_GPIOC_CLKEN;

    if (!RCC_GPIOD_CLK)
        RCC_GPIOD_CLKEN;

#ifdef HARDWARE_VERSION_2_0    
    //--- GPIOA Low Side ------------------//
    // PA0 Alternative TIM8_ETR input
    // PA1 NC
    // PA2 Analog Channel 2 (Sense_BOOST)
    // PA3 Analog Channel 3 (Sense_12V_EXT)
    // PA4 Analog DAC Output1 (DAC_OUT1)
    // PA5 NC
    // PA6 NC
    // PA7 Alternative (TIM3_CH2) open drain
    temp = GPIOA->CRL;
    temp &= 0x0FF000F0;
    temp |= 0xF0000004;
    GPIOA->CRL = temp;

    //--- GPIOA High Side ------------------//
    //PA8 Alternative (TIM1_CH1), starts on default
    //PA9 Alternative (TIM1_CH2)
    //PA10 NC
    //PA11 NC
    //PA12 Alternative TIM1_ETR input
    //PA13 NC
    //PA14 NC
    //PA15 NC
    temp = GPIOA->CRH;

    // PA8 starts alternative
    // temp &= 0xFFF0FF00;
    // temp |= 0x000400AF;

    // PA8 starts on default
    temp &= 0xFFF0FF0F;
    temp |= 0x000400A0;

    GPIOA->CRH = temp;

    //--- GPIOA Pull-Up Pull-Dwn ------------------//
    temp = GPIOA->ODR;
    temp &= 0xFFFF;   
    temp |= 0x0000;
    GPIOA->ODR = temp;

    //--- GPIOB Low Side -------------------//
    //PB0 ON_OFF
    //PB1 NC
    //PB2 NC
    //PB3 NC jtag
    //PB4 NC jtag
    //PB5 NC
    //PB6 NC
    //PB7 NC
    temp = GPIOB->CRL;
    temp &= 0xFFFFFFF0;
    temp |= 0x00000002;
    GPIOB->CRL = temp;

    //--- GPIOB High Side -------------------//
    //PB8 NC
    //PB9 NC
    //PB10 alternative Tx Usart3
    //PB11 alternative Rx Usart3
    //PB12 LED
    //PB13 NC
    //PB14 NC
    //PB15 NC
    temp = GPIOB->CRH;
    temp &= 0xFFF000FF;
    temp |= 0x00028B00;
    GPIOB->CRH = temp;    
    
    //--- GPIOB Pull-Up Pull-Dwn ------------------//
    temp = GPIOB->ODR;    //PB11 pull-up
    temp &= 0xF7FF;
    temp |= 0x0800;
    GPIOB->ODR = temp;

    //--- GPIOC Low Side -------------------//
    // PC0 Analog Channel 10 (Sense_BAT_CH1)
    // PC1 Analog Channel 11 (Sense_BAT_CH2)
    // PC2 Analog Channel 12 (Sense_BAT_CH3)
    // PC3 Analog Channel 13 (Sense_BAT_CH4)
    // PC4 NC
    // PC5 NC
    // PC6 Alterantive (TIM8_CH1), starts on default
    // PC7 NC

    temp = GPIOC->CRL;
    // PC6 starts alternative
    // temp &= 0xF0FF0000;
    // temp |= 0x0F000000;
    // PC6 starts on default
    temp &= 0xFFFF0000;
    temp |= 0x00000000;
    GPIOC->CRL = temp;

    //--- GPIOC High Side -------------------//
    //PC8 NC
    //PC9 NC
    //PC10 NC
    //PC11 NC
    //PC12 NC
    //PC13 NC
    //PC14 NC    oscillator
    //PC15 NC    oscillator
    temp = GPIOC->CRH;   
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOC->CRH = temp;

    //--- GPIOC Pull-Up Pull-Dwn ------------------//
    temp = GPIOC->ODR;
    temp &= 0xFFFF;
    temp |= 0x0000;
    GPIOC->ODR = temp;

    //--- GPIOD Low Side -------------------//
    //PD0 NC
    //PD1 NC
    //PD2 SW_POWER_ON
    //PD3 No implemented
    //PD4 No implemented
    //PD5 No implemented
    //PD6 No implemented
    //PD7 No implemented    
    temp = GPIOD->CRL;   
    temp &= 0xFFFFF0FF;
    temp |= 0x00000800;
    GPIOD->CRL = temp;

    //--- GPIOD Pull-Up Pull-Dwn ------------------//
    temp = GPIOD->ODR;    // PD2 pull-dwn
    temp &= 0xFFFB;
    temp |= 0x0000;
    GPIOD->ODR = temp;
    
#ifdef USE_EXTI_LINES
    //Interrupts on:
    // PB6 Rx bit bang pin
    if (!RCC_AFIO_CLK)
        RCC_AFIO_CLKEN;

    // EXTI6 Select Port B & Pin6 for external interrupt
    temp = AFIO->EXTICR[1];
    temp &= ~AFIO_EXTICR2_EXTI6;
    temp |= AFIO_EXTICR2_EXTI6_PB;
    AFIO->EXTICR[1] = (unsigned short) temp;

    // EXTI->IMR |= 0x00000001;    //Corresponding mask bit for interrupts EXTI2 EXTI4 EXTI13 EXTI15
    // EXTI->EMR |= 0x00000000;    //Corresponding mask bit for events
    EXTI->RTSR |= 0x00000000;
    EXTI->FTSR |= EXTI_FTSR_TR6;    //Interrupt line on falling edge

    // Enable NVIC for EXTIs
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    NVIC_SetPriority(EXTI9_5_IRQn, 3);
    
#endif    // USE_EXTI_LINES
#endif    // HARDWARE_VERSION_2_0

#ifdef HARDWARE_VERSION_1_0    
    //--- GPIOA Low Side ------------------//
    // PA0 ENA_ENCODER
    // PA1 NC
    // PA2 alternative Tx Usart2
    // PA3 alternative Rx Usart2
    // PA4 Analog Channel 4 (SENSE_12V_EXT)
    // PA5 Analog Channel 5 (SENSE_BAT_A)
    // PA6 Analog Channel 6 (SENSE_BAT_B)
    // PA7 NC
    temp = GPIOA->CRL;
    temp &= 0xF00000F0;
    temp |= 0x00008B02;
    GPIOA->CRL = temp;

    //--- GPIOA High Side ------------------//
    //PA8 ENA_BOOST
    //PA9 alternative Tx Usart1
    //PA10 alternative Rx Usart1
    //PA11 PROBE_SENSE_CH4
    //PA12 ACT_PROBE_CH4
    //PA13 NC
    //PA14 NC
    //PA15 NC
    temp = GPIOA->CRH;
    temp &= 0xFFF00000;
    temp |= 0x000288B2;
    GPIOA->CRH = temp;

    //--- GPIOA Pull-Up Pull-Dwn ------------------//
    temp = GPIOA->ODR;    //PA3 pull-up; PA11 pull-up    
    temp &= 0xF7F7;       // PA10 NO pull-up;
    temp |= 0x0808;
    GPIOA->ODR = temp;

    //--- GPIOB Low Side -------------------//
    //PB0 ACT_PROBE_CH2
    //PB1 SYNC_CH2
    //PB2 ENA_CH2
    //PB3 NC jtag
    //PB4 NC jtag
    //PB5 NC
    //PB6 Rx pin bit bang
    //PB7 Tx pin bit bang
    temp = GPIOB->CRL;
    temp &= 0x00FFF000;
    temp |= 0x24000222;
    GPIOB->CRL = temp;

    //--- GPIOB High Side -------------------//
    //PB8 SYNC_CH4
    //PB9 ENA_CH4
    //PB10 alternative Tx Usart3
    //PB11 alternative Rx Usart3
    //PB12 PROBE_SENSE_CH3
    //PB13 ACT_PROBE_CH3
    //PB14 SYNC_CH3
    //PB15 ENA_CH3
    temp = GPIOB->CRH;
    temp &= 0x00000000;
    temp |= 0x22288B22;
    GPIOB->CRH = temp;    
    
    //--- GPIOB Pull-Up Pull-Dwn ------------------//
    temp = GPIOB->ODR;    //PB11 pull-up; PB12 pull-up
    temp &= 0xE7FF;
    temp |= 0x1800;
    GPIOB->ODR = temp;

    //--- GPIOC Low Side -------------------//
    //PC0 PROBE_SENSE_CH1
    //PC1 ACT_PROBE_CH1
    //PC2 SYNC_CH1
    //PC3 ENA_CH1
    //PC4 NC
    //PC5 PROBE_SENSE_CH2
    //PC6 NC
    //PC7 ENA_5V_COMM
    temp = GPIOC->CRL;
    temp &= 0x0F0F0000;
    temp |= 0x20802228;
    GPIOC->CRL = temp;

    //--- GPIOC High Side -------------------//
    //PC8 ENA_LCD
    //PC9 ENA_RPI
    //PC10 Alternative Uart4 Tx
    //PC11 Alternative Uart4 Rx
    //PC12 Alternative Uart5 Tx
    //PC13 NC
    //PC14 NC    oscillator
    //PC15 NC    oscillator
    temp = GPIOC->CRH;   
    temp &= 0xFFF00000;
    temp |= 0x000B8B22;
    GPIOC->CRH = temp;

    //--- GPIOC Pull-Up Pull-Dwn ------------------//
    temp = GPIOC->ODR;    //PC0 pull-up; PC5 pull-up; PC11 pull-up
    temp &= 0xF7DE;
    temp |= 0x0821;
    GPIOC->ODR = temp;

    //--- GPIOD Low Side -------------------//
    //PD0 NC
    //PD1 NC
    //PD2 Alternative Uart5 Rx
    //PD3 No implemented
    //PD4 No implemented
    //PD5 No implemented
    //PD6 No implemented
    //PD7 No implemented    
    temp = GPIOD->CRL;   
    temp &= 0xFFFFF0FF;
    temp |= 0x00000800;
    GPIOD->CRL = temp;

    //--- GPIOD Pull-Up Pull-Dwn ------------------//
    temp = GPIOD->ODR;    //PC0 pull-up; PC5 pull-up; PC11 pull-up
    temp &= 0xFFFB;
    temp |= 0x0004;
    GPIOD->ODR = temp;
    
#ifdef USE_EXTI_LINES
    //Interrupts on:
    // PB6 Rx bit bang pin
    if (!RCC_AFIO_CLK)
        RCC_AFIO_CLKEN;

    // EXTI6 Select Port B & Pin6 for external interrupt
    temp = AFIO->EXTICR[1];
    temp &= ~AFIO_EXTICR2_EXTI6;
    temp |= AFIO_EXTICR2_EXTI6_PB;
    AFIO->EXTICR[1] = (unsigned short) temp;

    // EXTI->IMR |= 0x00000001;    //Corresponding mask bit for interrupts EXTI2 EXTI4 EXTI13 EXTI15
    // EXTI->EMR |= 0x00000000;    //Corresponding mask bit for events
    EXTI->RTSR |= 0x00000000;
    EXTI->FTSR |= EXTI_FTSR_TR6;    //Interrupt line on falling edge

    // Enable NVIC for EXTIs
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    NVIC_SetPriority(EXTI9_5_IRQn, 3);
    
#endif    // USE_EXTI_LINES
#endif    // HARDWARE_VERSION_1_0
}


#ifdef USE_EXTI_LINES
void EXTIOff (void)
{
    // EXTI->IMR &= ~(EXTI_IMR_MR2 | EXTI_IMR_MR4 | EXTI_IMR_MR13 | EXTI_IMR_MR15);
    EXTI->IMR &= ~(EXTI_IMR_MR6);
}

void EXTIOn (void)
{
    // EXTI->IMR |= (EXTI_IMR_MR2 | EXTI_IMR_MR4 | EXTI_IMR_MR13 | EXTI_IMR_MR15);
    EXTI->IMR |= (EXTI_IMR_MR6);
}
#endif


//--- end of file ---//
