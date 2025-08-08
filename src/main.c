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

#include "adc.h"
#include "dma.h"
#include "dac.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"

#include "comms.h"
#include "test_functions.h"

#include "supply.h"

#include <stdio.h>
#include <string.h>


// Private Types Constants and Macros ------------------------------------------


// Externals -------------------------------------------------------------------
//--- Externals from timers
volatile unsigned short timer_standby = 0;
volatile unsigned short wait_ms_var = 0;


//--- Externals from adc
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
void TimingDelay_Decrement(void);
void SysTickError (void);

#ifdef HARDWARE_VERSION_1_0
unsigned char Probe_Detect_Ch1 (void);
void Probe_Detect_Update (void);
void Starts_Everything (void);
void Shutdown_Everything (void);
void Full_Working_Loop (void);
#endif

// Module Functions ------------------------------------------------------------
int main (void)
{
    // Gpio Configuration.
    GpioInit();
    
    // Systick Timer Activation
    if (SysTick_Config(64000))
        SysTickError();

    // Hardware Tests
    // TF_Hardware_Tests ();

    // --- main program inits. ---
#ifdef HARDWARE_VERSION_2_0
    // --- start peripherals
    // Init ADC with DMA
    DMAConfig ();
    DMA_ENABLE;
    
    AdcConfig();
    AdcStart();
    
    //-- DAC init for signal generation
    DAC_Config ();
    DAC_Output1(0);

    // Init Usart3
    Usart3Config();

    // Init Tim3 for neopixel
    TIM3_Init();

    // Init Tim1 Tim8 for Boost
    // TIM1_Init_Master_Output_Disable ();
    // TIM8_Init_Slave_Output_Disable ();
    
    // --- start supply manager
    Usart3Send("\r\n -- Supply Board ver 2.0 init --\r\n");
    Supply_Status_Reset ();
    while (1)
    {
	Supply_Status ();

	Comms_Update ();
    }
#endif    // END OF HARDWARE_VERSION_2_0

    
#ifdef HARDWARE_VERSION_1_0
    //-- ADC without DMA
    AdcConfig();
    // AdcStart();
    
    supply_states_e supply_state = INIT;
    battery_status_e mains_status = STATUS_UNKNOWN;
    char buff_report [100];
        
    //-- Main Loop --------------------------
    while (1)
    {
        switch (supply_state)
        {
        case INIT:
            // check state every 200ms
            if (!timer_standby)
            {
                timer_standby = 200;
                mains_status = Battery_Check ();
                
                if (mains_status == RUNNING_ON_MAINS)
                {
                    // start everything
                    Starts_Everything ();
                    supply_state = MAINS_SUPPLY;
                }
                else if (mains_status == RUNNING_ON_BATTERY)
                {
                    // check battery voltage
                    if ((Battery_Get_BatA () > BATTERY_CONNECT_VOLTAGE) ||
                        (Battery_Get_BatB () > BATTERY_CONNECT_VOLTAGE))
                    {
                        // running on good bat, start everything
                        Starts_Everything ();
                        supply_state = BATTERY_GOOD;
                    }
                    else
                    {
                        // low batt
                        Shutdown_Everything ();
                        supply_state = BATTERY_LOW;
                    }
                }
            }
            break;

        case MAINS_SUPPLY:
            Full_Working_Loop ();

            if (!timer_standby)
            {
                timer_standby = 200;

                mains_status = Battery_Check ();
                
                if (mains_status == RUNNING_ON_BATTERY)
                    supply_state = BATTERY_GOOD;
            }

            // send batteries voltage every five secs
            if (!timer_for_batt_report)
            {
                unsigned char mains_int = 0;
                unsigned char batta_int = 0;
                unsigned char battb_int = 0;
                unsigned char mains_dec = 0;
                unsigned char batta_dec = 0;
                unsigned char battb_dec = 0;
                unsigned short adc_volts = 0;
                char status_batta;
                char status_battb;

                timer_for_batt_report = 5000;

                adc_volts = Battery_Get_Mains ();
                Battery_Convert_To_Volts (adc_volts, &mains_int, &mains_dec);

                adc_volts = Battery_Get_BatA ();
                status_batta = Battery_Convert_Status_From_Adc (adc_volts);
                Battery_Convert_To_Volts (adc_volts, &batta_int, &batta_dec);

                adc_volts = Battery_Get_BatB ();
                status_battb = Battery_Convert_Status_From_Adc (adc_volts);
                Battery_Convert_To_Volts (adc_volts, &battb_int, &battb_dec);

                sprintf(buff_report, "supply mains %d.%01dV %d.%01dV %d.%01dV %c %c\r\n",
                        mains_int,
                        mains_dec,
                        battb_int,
                        battb_dec,
                        batta_int,
                        batta_dec,
                        status_battb,
                        status_batta);

                UsartRpiSend(buff_report);                
            }
            break;

        case BATTERY_GOOD:
            Full_Working_Loop ();

            if (!timer_standby)
            {
                timer_standby = 200;

                mains_status = Battery_Check ();
                
                if (mains_status == RUNNING_ON_MAINS)
                    supply_state = MAINS_SUPPLY;

                // check battery voltage
                if ((Battery_Get_BatA () < BATTERY_DISCONNECT_VOLTAGE) &&
                    (Battery_Get_BatB () < BATTERY_DISCONNECT_VOLTAGE))
                {
                    // low batt
                    Shutdown_Everything ();
                    supply_state = BATTERY_LOW;
                }
            }

            // send batteries voltage every five secs
            if (!timer_for_batt_report)
            {
                unsigned char batta_int = 0;
                unsigned char battb_int = 0;
                unsigned char batta_dec = 0;
                unsigned char battb_dec = 0;
                unsigned short adc_volts = 0;
                char status_batta;
                char status_battb;

                timer_for_batt_report = 5000;

                adc_volts = Battery_Get_BatA ();
                status_batta = Battery_Convert_Status_From_Adc (adc_volts);
                Battery_Convert_To_Volts (adc_volts, &batta_int, &batta_dec);

                adc_volts = Battery_Get_BatB ();
                status_battb = Battery_Convert_Status_From_Adc (adc_volts);
                Battery_Convert_To_Volts (adc_volts, &battb_int, &battb_dec);

                sprintf(buff_report, "supply battery 0.0V %d.%01dV %d.%01dV %c %c\r\n",
                        battb_int,
                        battb_dec,
                        batta_int,
                        batta_dec,
                        status_battb,
                        status_batta);

                UsartRpiSend(buff_report);                
            }
            break;

        case BATTERY_LOW:
            if (!timer_standby)
            {
                timer_standby = 1000;

                mains_status = Battery_Check ();
                
                if (mains_status == RUNNING_ON_MAINS)
                {
                    // mains connection re-stablish
                    // start everything
                    Starts_Everything ();
                    supply_state = MAINS_SUPPLY;
                }

                // check battery voltage
                if ((Battery_Get_BatA () > BATTERY_CONNECT_VOLTAGE) ||
                    (Battery_Get_BatB () > BATTERY_CONNECT_VOLTAGE))
                {
                    // battery charged externally???
                    // running on good bat, start everything
                    Starts_Everything ();
                    supply_state = BATTERY_GOOD;
                }
            }            
            break;
        }


        // Always check battery status
        Battery_Status ();
    }
#endif    // HARDWARE_VERSION_1_0
}

//--- End of Main ---//


// Other Module Functions ------------------------------------------------------
#ifdef HARDWARE_VERSION_1_0
#define SIZE_BUFFRX    124
void Full_Working_Loop (void)
{
    char buff [SIZE_BUFFRX];
    char buff_tx [128];

    // update the channels & rpi comms
    Comms_Update ();

    // channels bridge
    if (UsartChannel1HaveData())
    {
        UsartChannel1HaveDataReset();
        UsartChannel1ReadBuffer(buff, SIZE_BUFFRX);
        sprintf(buff_tx,"ch1 %s\n", buff);
        UsartRpiSend(buff_tx);
    }

    if (UsartChannel2HaveData())
    {
        UsartChannel2HaveDataReset();
        UsartChannel2ReadBuffer(buff, SIZE_BUFFRX);
        sprintf(buff_tx,"ch2 %s\n", buff);
        UsartRpiSend(buff_tx);
    }

    if (UsartChannel3HaveData())
    {
        UsartChannel3HaveDataReset();
        UsartChannel3ReadBuffer(buff, SIZE_BUFFRX);
        sprintf(buff_tx,"ch3 %s\n", buff);
        UsartRpiSend(buff_tx);
    }

    if (UsartChannel4HaveData())
    {
        UsartChannel4HaveDataReset();
        UsartChannel4ReadBuffer(buff, SIZE_BUFFRX);
        sprintf(buff_tx,"ch4 %s\n", buff);
        UsartRpiSend(buff_tx);
    }
    
    // rx from I2C
    // i2c_driver_update ();

    // rx on encoders board
    if (Bit_Bang_Rx_Have_Data())
    {
        unsigned char len;
        len = Bit_Bang_Rx_ReadBuffer(buff);
        if (len < SIZE_BUFFRX - 1)
        {
            buff[len] = '\n';
            buff[len + 1] = '\0';
            UsartRpiSend(buff);
        }
    }
        
    // enable channel on probe detection
    Probe_Detect_Update ();

    // channels enable and disable by interface
    // check ch1 enable
    // if (Probe_Detect_Ch1 ())
    //     Ena_Ch1_On();
    // else
    //     Ena_Ch1_Off();
    
}


void Starts_Everything (void)
{
    //-- Enable Boost Supply
    ENA_BOOST_ON;
    
    //-- Starts with all channels disabled
    Ena_Ch1_Off();
    Ena_Ch2_Off();
    Ena_Ch3_Off();
    Ena_Ch4_Off();

    //-- Shutdown RPi and Encoders Board
    ENA_ENCODER_OFF;
    ENA_RPI_OFF;

    //-- Boot on RPi
    Wait_ms(300);
    ENA_RPI_ON;

    //-- Boot on LCD Display
    Wait_ms(100);
    ENA_LCD_ON;
    
    //-- Boot Encoders Board
    Wait_ms(300);
    ENA_ENCODER_ON;

    //-- Comms with encoders board
    // I2C1_Init();
    // I2C1_OwnAddress (0x44);
    // I2C1_Ack (1);
    Bit_Bang_Init();

    //-- Comms with rasp & channels
    UsartRpiConfig ();
    UsartChannel1Config ();
    UsartChannel2Config ();
    UsartChannel3Config ();
    UsartChannel4Config ();

    //-- Enable 5V COMM supply
    ENA_5V_COMM_ON;
    
    //-- Enable 5V to comms in channels
    Act_Probe_Ch1_On ();
    Act_Probe_Ch2_On ();
    Act_Probe_Ch3_On ();
    Act_Probe_Ch4_On ();    
}


void Shutdown_Everything (void)
{
    //-- Shutdown Encoders Board
    ENA_ENCODER_OFF;

    //-- Comms with encoders board
    // I2C1_Shutdown ();
    Bit_Bang_Shutdown();
    
    //-- All channels disabled
    Ena_Ch1_Off();
    Ena_Ch2_Off();
    Ena_Ch3_Off();
    Ena_Ch4_Off();

    //-- Comms with rasp & channels
    UsartRpiShutdown ();
    UsartChannel1Shutdownn ();
    UsartChannel2Shutdownn ();
    UsartChannel3Shutdownn ();
    UsartChannel4Shutdownn ();

    //-- Lcd Display Shutdown
    ENA_LCD_OFF;
    
    //-- RPi Shutdown
    ENA_RPI_OFF;

    //-- Disable 5V to comms in channels
    Act_Probe_Ch1_Off ();
    Act_Probe_Ch2_Off ();
    Act_Probe_Ch3_Off ();
    Act_Probe_Ch4_Off ();

    //-- Disable 5V COMM supply
    ENA_5V_COMM_OFF;

    //-- Disable Boost Supply
    ENA_BOOST_OFF;
    
}


unsigned char Probe_Detect_Ch1 (void)
{
    if (probe_detect_filter > 60)
        return 1;

    return 0;
}


void Probe_Detect_Update (void)
{
    if (!probe_detect_timer)
    {
        probe_detect_timer = 1;

        if (PROBE_SENSE_CH1)
        {
            if (probe_detect_filter < 200)
                probe_detect_filter += 20;
        }
        else
        {
            if (probe_detect_filter)
                probe_detect_filter--;
        }
    }
}


void EXTI9_5_IRQHandler (void)
{
    if(EXTI->PR & EXTI_PR_PR6)    //Line6
    {
        EXTI->PR |= EXTI_PR_PR6;
        Bit_Bang_Rx_Int_Handler ();
    }
}


// void EXTI4_IRQHandler (void)
// {
//     if(EXTI->PR & EXTI_PR_PR4)    //Line4
//     {
//         Signals_Overcurrent_Handler (CH4);        
//         // TF_Prot_Int_Handler (4);    // PROT_CH4 for tests
//         EXTI->PR |= EXTI_PR_PR4;
//     }
// }


// void EXTI15_10_IRQHandler (void)
// {
//     if(EXTI->PR & EXTI_PR_PR13)    //Line13
//     {
//         Signals_Overcurrent_Handler (CH2);
//         // TF_Prot_Int_Handler (2);    // PROT_CH2 for tests
//         EXTI->PR |= EXTI_PR_PR13;
//     }
//     else if (EXTI->PR & EXTI_PR_PR15)    //Line15
//     {
//         Signals_Overcurrent_Handler (CH1);        
//         // TF_Prot_Int_Handler (1);    // PROT_CH1 for tests
//         EXTI->PR |= EXTI_PR_PR15;
//     }
// }


#endif    // HARDWARE_VERSION_1_0

void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    // if (timer_for_batt_report)
    //     timer_for_batt_report--;    

    // if (probe_detect_timer)
        // probe_detect_timer--;

    // i2c_driver_timeouts ();

    Hard_Timeouts ();
    
    Supply_Timeouts ();
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

