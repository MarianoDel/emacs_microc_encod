//---------------------------------------------------------------
// #### PROJECT MICRO-CURRENTS SUPPLY-BRD F103 - Custom Board ###
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### MAIN.C ##################################################
//---------------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "stm32f10x.h"
#include "hard.h"

#include "tim.h"
#include "gpio.h"
#include "usart_channels.h"

#include "comms_main.h"
#include "test_functions.h"

#include <stdio.h>
#include <string.h>


// Private Types Constants and Macros ------------------------------------------


// Externals -------------------------------------------------------------------
//--- Externals from timers
volatile unsigned short timer_standby = 0;
volatile unsigned short wait_ms_var = 0;



// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
void TimingDelay_Decrement(void);
void SysTickError (void);


// Module Functions ------------------------------------------------------------
int main (void)
{
    // Gpio Configuration.
    GpioInit();
    
    // Systick Timer Activation
    if (SysTick_Config(64000))
        SysTickError();

    // Hardware Tests
    TF_Hardware_Tests ();

    // --- main program inits. ---

    // Init Usart to Mainboard
    UsartMainConfig ();
    
    UsartMainSend ("\r\n -- Encoders Board ver 2.0 init --\r\n");

    
    while (1)
    {
	// Supply_Status ();

	// Comms_Update ();
    }
}

//--- End of Main ---//


// Other Module Functions ------------------------------------------------------
void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    Usart_Timeouts ();
    
}

void SysTickError (void)
{
    //Capture systick error...
    while (1)
    {
        // if (LED)
        //     LED_OFF;
        // else
        //     LED_ON;

        for (unsigned char i = 0; i < 255; i++)
        {
            asm ("nop \n\t"
                 "nop \n\t"
                 "nop \n\t" );
        }
    }
}

//--- end of file ---//

