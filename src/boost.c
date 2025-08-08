//------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### BOOST.C ###############################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "boost.h"
#include "hard.h"
#include "dma.h"
#include "tim.h"
#include "dac.h"
#include "dsp.h"
#include "supply.h"
#include "adc.h"


// Module Private Types Constants & Macros -------------------------------------


// Externals -------------------------------------------------------------------
extern volatile unsigned short adc_ch [];


// Globals ---------------------------------------------------------------------
pid_data_obj_t pid_small;


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void Boost_Start (void)
{
    TIM1_Init_Master_Output_Disable ();
    TIM8_Init_Slave_Output_Disable ();

    DAC_Output1(1000);    // max 12.7V con 4A carga
    // DAC_Output1(1500);    // max 12.7V con 4A carga cortan los BMS con una sola batt
    TIM1_Update_CH1 (0);
    TIM8_Update_CH1 (0);

    TIM1_Output_Enable ();
    TIM8_Output_Enable ();

    pid_small.kp = 1;
    pid_small.ki = 3;
    pid_small.kd = 0;
    PID_Small_Ki_Flush_Errors (&pid_small);
}


void Boost_Update (void)
{
    if (!sequence_ready)
	return;
    
    sequence_ready_reset;

    if ((Supply_Get_Boost() < BOOST_14_5V) &&
	(Sense_BOOST < BOOST_15V))
    {
	short d = 0;
	pid_small.setpoint = BOOST_13_8V;
	pid_small.sample = Sense_BOOST;	
	d = PID_Small_Ki(&pid_small);

	if (d > 900)
	{
	    d = 900;
	    pid_small.last_d = 900;
	}
	else if (d < 0)
	{
	    d = 0;
	    pid_small.last_d = 0;
	}
	
	TIM1_Update_CH1 (d);
	TIM8_Update_CH1 (d);
    }
    else
    {
	PID_Small_Ki_Flush_Errors(&pid_small);
	TIM1_Update_CH1 (0);
	TIM8_Update_CH1 (0);
    }
    
    if (Led_Is_On())
	Led_Off();
    else
	Led_On();
}


void Boost_Stop (void)
{
    TIM1_Output_Disable ();
    TIM8_Output_Disable ();

    TIM1_Update_CH1 (0);
    TIM8_Update_CH1 (0);

    TIM1_DeInit();
    TIM8_DeInit();

    Led_Off();
}


//--- end of file ---//
