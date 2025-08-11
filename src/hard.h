//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### HARD.H #################################
//---------------------------------------------

#ifndef HARD_H_
#define HARD_H_


//----------- Defines For Configuration -------------

//----- Board Configuration -------------------//
//--- Hardware ------------------//
#define HARDWARE_VERSION_2_0    // second prototype encoder + display
// #define HARDWARE_VERSION_1_0    // first prototype


//--- Software ------------------//
#define FIRMWARE_VERSION_1_0    // init version


//-------- Type of Program (depending on software version) ----------------


//--- Serial Number / Device Id Bytes length ----------
#define USE_DEVICE_ID_4BYTES
// #define USE_DEVICE_ID_12BYTES



//-------- Oscillator and Crystal selection (Freq in startup_clocks.h) ---
#define HSI_INTERNAL_RC
// #define HSE_CRYSTAL_OSC

#ifdef HSE_CRYSTAL_OSC
// #define CRYSTAL_8MHZ
#define CRYSTAL_12MHZ
#endif

#ifdef HSE_CRYSTAL_OSC
// #define SYSCLK_FREQ_72MHz
#define SYSCLK_FREQ_8MHz
#endif

#ifdef HSI_INTERNAL_RC
#define SYSCLK_FREQ_64MHz
// #define SYSCLK_FREQ_8MHz
#endif

//-------- End Of Defines For Configuration ------



//--- Hardware & Software Messages ------------------//
#ifdef HARDWARE_VERSION_2_0
#define HARD "Hardware Version: 2.0"
#endif
#ifdef HARDWARE_VERSION_1_0
#define HARD "Hardware Version: 1.0"
#endif
#ifdef FIRMWARE_VERSION_1_0
#define SOFT "Firmware Version: 1.0"
#endif
//--- End of Hardware & Software Messages ------------------//



// Exported Types --------------------------------------------------------------
// PA defines ----
// PA0 NC
// PA1 NC

// PA2 Alternative (USART2_TX)
// PA3 Alternative (USART2_RX)

// PA4 NC
// PA5 NC
// PA6 NC
// PA7 NC
// PA8 NC

// PA9 Alternative (USART1_TX)
// PA10 Alternative (USART1_RX)

// PA11 NC
// PA12 NC
// PA13 PA14 PA15 NC jtag

// PB defines ----
// PB0 NC
// PB1 NC
// PB2 NC
// PB3 NC
// PB4 NC jtag
// PB5 NC
// PB6 NC
// PB7 NC
// PB8 NC
// PB9 NC

// PB10 Alternative (USART3_TX)
// PB11 Alternative (USART3_RX)

// PB12 NC
// PB13 NC
// PB14 NC
// PB15 NC

// PC defines ----
// PC0 NC
// PC1 NC
// PC2 NC
// PC3 NC
// PC4 NC
// PC5 NC
// PC6 NC
// PC7 NC
// PC8 NC
// PC9 NC

// PC10 Alternative (UART4_TX)
// PC11 Alternative (UART4_RX)

// PC12 Alternative (UART5_TX)
// PC13 PC14 PC15 NC

// PD defines ----
// PD0 PD1 NC

// PD2 Alternative (UART5_RX)


//--- Module Exported Functions ------------------------------------------------


#endif    /* HARD_H_ */
