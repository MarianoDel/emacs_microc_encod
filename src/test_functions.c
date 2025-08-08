//----------------------------------------------------------
// #### MAGNET PROJECT - Custom Board ####
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.C ###################################
//----------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "test_functions.h"
#include "hard.h"
#include "stm32f10x.h"
#include "gpio.h"
#include "adc.h"
#include "dac.h"
#include "usart.h"
#include "dma.h"
#include "tim.h"

#include "neopixel.h"
#include "neopixel_driver.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------
extern volatile unsigned short adc_ch [];
extern volatile unsigned short wait_ms_var;
extern volatile unsigned short timer_standby;
extern volatile unsigned char timer_6_uif_flag;
extern volatile unsigned char rx_int_handler;
// extern volatile unsigned char usart3_have_data;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
#ifdef HARDWARE_VERSION_2_0
void TF_Led (void);
void TF_Led_Dac (void);
void TF_Led_Sw_Power_On (void);
void TF_Led_OnOff (void);
void TF_Usart3_Loop (void);
void TF_Usart3_Analog (void);
void TF_Tim3_Ch2_NeoPixel_1_Enable (void);
void TF_Tim1_Master_Tim8_Slave (void);
#endif

#ifdef HARDWARE_VERSION_1_0
void TF_Act_Channels (void);
void TF_Enable_Channels (void);
void TF_Synchro_Channels (void);
void TF_Enable_5V_Comm (void);
void TF_Enable_Lcd (void);
void TF_Enable_Rpi (void);
void TF_Enable_Encoder (void);
void TF_Enable_Boost (void);


void TF_UsartChannel1_Loop (void);
void TF_UsartChannel2_Loop (void);
void TF_UsartChannel3_Loop (void);
void TF_UsartChannel4_Loop (void);

void TF_UsartRpi_Loop (void);
void TF_UsartRpi_String (void);

void TF_PowerOn_Channel1_Channel2 (void);

void TF_Tim6_Int (void);
void TF_Bit_Bang_Send (void);
void TF_Bit_Bang_Loop (void);

void TF_Int_Pb6 (void);
// void TF_Adc_Usart1_Tx (void);
// void TF_Adc_Usart1_Voltages (void);
#endif

// Module Functions ------------------------------------------------------------
void TF_Hardware_Tests (void)
{
    // TF_Led ();
    // TF_Led_Dac ();
    // TF_Led_Sw_Power_On ();
    // TF_Led_OnOff ();
    // TF_Usart3_Loop ();
    // TF_Usart3_Analog ();
    // TF_Tim3_Ch2_NeoPixel_1_Enable ();
    TF_Tim1_Master_Tim8_Slave ();
}


void TF_Led (void)
{
    while (1)
    {
	if (Led_Is_On())
	    Led_Off();
	else
	    Led_On();

	Wait_ms (1000);
    }
}


void TF_Led_Dac (void)
{
    //-- DAC init for signal generation
    DAC_Config ();
    DAC_Output1(0);

    while (1)
    {
	if (Led_Is_On())
	{
	    Led_Off();
	    DAC_Output1 (0);
	}
	else
	{
	    Led_On();
	    DAC_Output1 (2047);
	}

	Wait_ms (5000);
    }
}


void TF_Led_Sw_Power_On (void)
{
    while (1)
    {
	if (Sw_Power_On())
	    Led_On();
	else
	    Led_Off();

	Wait_ms (100);
    }
}


void TF_Led_OnOff (void)
{

    while (1)
    {
	if (OnOff_Is_On())
	{
	    Led_Off();
	    OnOff_Off();
	}
	else
	{
	    Led_On();
	    OnOff_On();
	}

	Wait_ms (5000);
    }
}


// place a shortcut on IC4 2 & IC3 4
void TF_Usart3_Loop (void)
{
    char buff [100];
    
    Usart3Config();
    
    while (1)
    {
        if (!timer_standby)
        {
            Usart3Send("Mariano\n");
            timer_standby = 2000;
            if (Led_Is_On())
                Led_Off();
        }

        if (Usart3HaveData())
        {
            Usart3HaveDataReset();
            Usart3ReadBuffer(buff, 100);
            if (strncmp(buff, "Mariano", sizeof("Mariano") - 1) == 0)
                Led_On();
        }
    }
}


void TF_Usart3_Analog (void)
{
    char buff [100];

    // Init ADC with DMA
    DMAConfig ();
    DMA_ENABLE;
    
    AdcConfig();
    AdcStart();
    
    // Init Usart3
    Usart3Config();
    
    while (1)
    {
        if (!timer_standby)
        {
	    Led_On();
	    timer_standby = 2000;
	    sprintf(buff, "b1: %d b2: %d b3: %d b4: %d 12v_ext: %d boost: %d\r\n",
		    Sense_BAT_CH1,
		    Sense_BAT_CH2,
		    Sense_BAT_CH3,
		    Sense_BAT_CH4,
		    Sense_12V_EXT,
		    Sense_BOOST);
            Usart3Send(buff);
	    Led_Off();
        }
    }
}


void TF_Tim3_Ch2_NeoPixel_1_Enable (void)
{
    unsigned char cnt = 0;
    pixel_t my_pixel;

    TIM3_Init();

    my_pixel.R = 0;
    my_pixel.G = 0;
    my_pixel.B = 0;

    // start entire buffer with null color
    Neo_Set_Pixel(0, &my_pixel);

    while (1)
    {
	Led_On();
	switch (cnt)
	{
	case 0:
	    cnt++;
	    my_pixel.R = 255;
	    my_pixel.G = 0;
	    my_pixel.B = 0;
	    break;
	    
	case 1:
	    cnt++;
	    my_pixel.R = 0;
	    my_pixel.G = 255;
	    my_pixel.B = 0;
	    break;

	case 2:
	    cnt = 0;
	    my_pixel.R = 0;
	    my_pixel.G = 0;
	    my_pixel.B = 255;
	    break;
	}
	
	Neo_Set_Pixel(0, &my_pixel);
	Neo_Driver_Send_Pixel_Data();
	Wait_ms(100);
	Led_Off();
	Wait_ms(1900);	
    }
}


void TF_Tim1_Master_Tim8_Slave (void)
{
    Wait_ms(10000);
    
    // slave clock enabled before master
    // Wait_ms(2);
    TIM1_Init_Master_Output_Disable ();
    // TIM1_Init();    
    // TIM2_Init();
    TIM8_Init_Slave_Output_Disable ();

    TIM1_Update_CH1 (0);
    TIM8_Update_CH1 (0);

    Wait_ms(2);
    
    TIM1_Output_Enable(); 
    TIM8_Output_Enable();   

    TIM1_Update_CH1 (100);
    TIM8_Update_CH1 (100);

    Led_On();
    
    while (1);
}


#ifdef HARDWARE_VERSION_1_0
void TF_Act_Channels (void)
{
    while (1)
    {
        Wait_ms(1000);

        if (ACT_PROBE_CH1)
        {
            ACT_PROBE_CH1_OFF;
            ACT_PROBE_CH2_OFF;
            ACT_PROBE_CH3_OFF;
            ACT_PROBE_CH4_OFF;            
        }
        else
        {
            ACT_PROBE_CH1_ON;
            ACT_PROBE_CH2_ON;
            ACT_PROBE_CH3_ON;
            ACT_PROBE_CH4_ON;            
        }
    }
}


void TF_Enable_Channels (void)
{
    while (1)
    {
        Wait_ms(1000);

        if (ENA_CH1)
        {
            ENA_CH1_OFF;
            ENA_CH2_OFF;
            ENA_CH3_OFF;
            ENA_CH4_OFF;            
        }
        else
        {
            ENA_CH1_ON;
            ENA_CH2_ON;
            ENA_CH3_ON;
            ENA_CH4_ON;            
        }
    }
}


void TF_Synchro_Channels (void)
{
    while (1)
    {
        Wait_ms(1000);

        if (SYNC_CH1)
        {
            SYNC_CH1_OFF;
            SYNC_CH2_OFF;
            SYNC_CH3_OFF;
            SYNC_CH4_OFF;            
        }
        else
        {
            SYNC_CH1_ON;
            SYNC_CH2_ON;
            SYNC_CH3_ON;
            SYNC_CH4_ON;            
        }
    }
}


void TF_Enable_5V_Comm (void)
{
    while (1)
    {
        ENA_5V_COMM_ON;
        Wait_ms(5000);
        Act_Probe_Ch1_On();
        Wait_ms(15000);
        ENA_5V_COMM_OFF;
        Act_Probe_Ch1_Off();
        Wait_ms(20000);

    }
}


void TF_Enable_Lcd (void)
{
    while (1)
    {
        Wait_ms(1000);

        if (ENA_LCD)
        {
            ENA_LCD_OFF;
        }
        else
        {
            ENA_LCD_ON;
        }
    }
}


void TF_Enable_Rpi (void)
{
    while (1)
    {
        Wait_ms(1000);

        if (ENA_RPI)
        {
            ENA_RPI_OFF;
        }
        else
        {
            ENA_RPI_ON;
        }
    }
}


void TF_Enable_Encoder (void)
{
    while (1)
    {
        Wait_ms(1000);

        if (ENA_ENCODER)
        {
            ENA_ENCODER_OFF;
        }
        else
        {
            ENA_ENCODER_ON;
        }
    }
}


void TF_Enable_Boost (void)
{
    while (1)
    {
        Wait_ms(1000);

        if (ENA_BOOST)
        {
            ENA_BOOST_OFF;
        }
        else
        {
            ENA_BOOST_ON;
        }
    }
}


// place a shortcut on IC4 2 & IC3 4
void TF_UsartChannel1_Loop (void)
{
    char buff [100];
    
    UsartChannel1Config();
    
    while (1)
    {
        if (!timer_standby)
        {
            UsartChannel1Send("Mariano\n");
            timer_standby = 2000;
            if (SYNC_CH1)
                SYNC_CH1_OFF;
        }

        if (UsartChannel1HaveData())
        {
            UsartChannel1HaveDataReset();
            UsartChannel1ReadBuffer(buff, 100);
            if (strncmp(buff, "Mariano", sizeof("Mariano") - 1) == 0)
                SYNC_CH1_ON;
        }
    }
}


// place a shortcut on IC4 2 & IC3 4
void TF_UsartChannel2_Loop (void)
{
    char buff [100];
    
    UsartChannel2Config();
    
    while (1)
    {
        if (!timer_standby)
        {
            UsartChannel2Send("Mariano\n");
            timer_standby = 2000;
            if (SYNC_CH1)
                SYNC_CH1_OFF;
        }

        if (UsartChannel2HaveData())
        {
            UsartChannel2HaveDataReset();
            UsartChannel2ReadBuffer(buff, 100);
            if (strncmp(buff, "Mariano", sizeof("Mariano") - 1) == 0)
                SYNC_CH1_ON;
        }
    }
}


// place a shortcut on IC4 2 & IC3 4
void TF_UsartChannel3_Loop (void)
{
    char buff [100];
    
    UsartChannel3Config();
    
    while (1)
    {
        if (!timer_standby)
        {
            UsartChannel3Send("Mariano\n");
            timer_standby = 2000;
            if (SYNC_CH1)
                SYNC_CH1_OFF;
        }

        if (UsartChannel3HaveData())
        {
            UsartChannel3HaveDataReset();
            UsartChannel3ReadBuffer(buff, 100);
            if (strncmp(buff, "Mariano", sizeof("Mariano") - 1) == 0)
                SYNC_CH1_ON;
        }
    }
}


// place a shortcut on IC4 2 & IC3 4
void TF_UsartChannel4_Loop (void)
{
    char buff [100];
    
    UsartChannel4Config();
    
    while (1)
    {
        if (!timer_standby)
        {
            UsartChannel4Send("Mariano\n");
            timer_standby = 2000;
            if (SYNC_CH1)
                SYNC_CH1_OFF;
        }

        if (UsartChannel4HaveData())
        {
            UsartChannel4HaveDataReset();
            UsartChannel4ReadBuffer(buff, 100);
            if (strncmp(buff, "Mariano", sizeof("Mariano") - 1) == 0)
                SYNC_CH1_ON;
        }
    }
}


// place a shortcut Rx Tx on Rpi connector
void TF_UsartRpi_Loop (void)
{
    char buff [100];
    
    UsartRpiConfig ();
    
    while (1)
    {
        if (!timer_standby)
        {
            UsartRpiSend ("Mariano\n");
            timer_standby = 2000;
            if (SYNC_CH1)
                SYNC_CH1_OFF;
        }

        if (UsartRpiHaveData ())
        {
            UsartRpiHaveDataReset ();
            UsartRpiReadBuffer (buff, 100);
            if (strncmp(buff, "Mariano", sizeof("Mariano") - 1) == 0)
                SYNC_CH1_ON;
        }
    }
}


// Terminal Looping on Rpi connector
void TF_UsartRpi_String (void)
{
    char buff [100];

    UsartRpiConfig ();
    UsartRpiSend("rpi usart test... send a string:\n");
    
    while (1)
    {
        if (UsartRpiHaveData())
        {
            UsartRpiHaveDataReset();
            UsartRpiReadBuffer(buff, 100);

            Wait_ms(1000);

            int i = strlen(buff);
            if (i < 99)
            {
                buff[i] = '\n';
                buff[i+1] = '\0';
                UsartRpiSend(buff);
            }
        }
    }
}


void TF_PowerOn_Channel1_Channel2 (void)
{
    ENA_BOOST_ON;
    ENA_CH1_ON;
    ENA_CH2_ON;
    
    while (1);
}


// Bit Bang Tests Functions
void TF_Bit_Bang_Send (void)
{
    // char buff [100];
    Bit_Bang_Init ();
    timer_standby = 5000;
    
    while (1)
    {
        if (!timer_standby)
        {
            Bit_Bang_Tx_Send ("enc 0 p 1\n");
            // Bit_Bang_Tx_Send ("Ma");
            timer_standby = 1000;
        }
    }
}


void TF_Bit_Bang_Loop (void)
{
    char buff [100];

    Bit_Bang_Init ();
    Bit_Bang_Tx_Send ("\nRx 1200 test\n");
    
    while (1)
    {
        if (Bit_Bang_Rx_Have_Data())
        {
            unsigned char len = 0;
            // memset(buff, 0, 100);
            len = Bit_Bang_Rx_ReadBuffer(buff);

            Wait_ms(1000);

            if (len < 98)
            {
                buff[len] = '\n';
                buff[len + 1] = '\0';
                Bit_Bang_Tx_Send(buff);
            }
        }
    }
}


void TF_Tim6_Int (void)
{
    // char buff [100];
    TIM6_Init ();
    TIM6_Start();
    // Bit_Bang_Init ();
    
    while (1)
    {
        if (timer_6_uif_flag)
        {
            timer_6_uif_flag = 0;
            if (PB7)
                PB7_OFF;
            else
                PB7_ON;
        }
    }
}


void TF_Int_Pb6 (void)
{
    Wait_ms(1000);
    
    EXTIOn();
    
    while (1)
    {
        if (rx_int_handler)
        {
            rx_int_handler = 0;
            if (PB7)
                PB7_OFF;
            else
                PB7_ON;
        }
    }
}
#endif    // HARDWARE_VERSION_1_0


//--- end of file ---//
