//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "bit_bang.h"

// helper modules
// #include "tests_ok.h"
// #include "tests_mock_usart.h"

#include <stdio.h>
#include <string.h>


// Types Constants and Macros --------------------------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Functions to Test ----------------------------------------------------
void Test_Bit_Bang (void);


// Module Auxiliary Functions --------------------------------------------------



// Tests Module Auxiliary or General Functions ---------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{

    Test_Bit_Bang ();

    return 0;
}


void Test_Bit_Bang (void)
{
}


// void Test_Comms_With_Usart (void)
// {
//     // set callback on usart1 RpiSend()
//     Usart1Callback(CB_Usart);


//     cb_usart_value = 0;
//     Usart2FillRxBuffer("loco\r\n");    
//     Comms_Channel1 ();

//     printf("Test error string on channel 1: ");
//     if (cb_usart_value == 0)    //no answer
//         PrintOK();
//     else
//         PrintERR();


//     // cb_usart_value = 0;    
//     // Usart2FillRxBuffer("name:Tunel\r\n");
//     // Comms_Channel1 ();
    
//     // printf("Test antenna name channel 1: ");
//     // if (cb_usart_value == 2)
//     //     PrintOK();
//     // else
//     //     PrintERR();


//     char my_ant_str [] = { "ant3,003.50,019.00,003.50,065.00\r\n" };
//     cb_usart_value = 0;    
//     Usart2FillRxBuffer(my_ant_str);
//     Comms_Channel1 ();
//     global_ant.resistance_int = 0;
//     global_ant.resistance_dec = 0;
//     global_ant.inductance_int = 0;
//     global_ant.inductance_dec = 0;

    
//     printf("Test old antenna & parse: ");
//     if ((cb_usart_value == 4) &&
//         (global_ant.resistance_int == 0) &&
//         (global_ant.resistance_dec == 0) &&
//         (global_ant.inductance_int == 0) &&
//         (global_ant.inductance_dec == 0))
//     {
//         PrintOK();
//     }
//     else
//         PrintERR();

//     cb_usart_value = 0;    
//     Usart2FillRxBuffer("temp,055.55\r\n");
//     Comms_Channel1 ();
    
//     printf("Test antenna current temp: ");
//     if (cb_usart_value == 2)
//         PrintOK();
//     else
//         PrintERR();

//     cb_usart_value = 0;
//     antenna_answered = 0;    
//     Usart2FillRxBuffer("ok\r\n");
//     Comms_Channel1 ();
    
//     printf("Test antenna keepalive answer: ");
//     if ((cb_usart_value == 0) && (antenna_answered))
//         PrintOK();
//     else
//         PrintERR();

//     char my_ant_str2 [] = { "ant3,0aa.50,019.00,003.50,065.50\r\n" };    
//     cb_usart_value = 0;    
//     Usart2FillRxBuffer(my_ant_str2);
//     Comms_Channel1 ();
    
//     global_ant.resistance_int = 0;
//     global_ant.resistance_dec = 0;
//     global_ant.inductance_int = 0;
//     global_ant.inductance_dec = 0;
    
//     printf("Test new antenna with errors on params: ");
//     if ((cb_usart_value == 3) &&
//         (global_ant.resistance_int == 0) &&
//         (global_ant.resistance_dec == 0) &&
//         (global_ant.inductance_int == 0) &&
//         (global_ant.inductance_dec == 0))
//         PrintOK();
//     else
//         PrintERR();

//     cb_usart_value = 0;    
//     Usart2FillRxBuffer("temp,0aa.55\r\n");
//     Comms_Channel1 ();
    
//     printf("Test antenna error on temp integer: ");
//     if (cb_usart_value == 0)
//         PrintOK();
//     else
//         PrintERR();

//     cb_usart_value = 0;    
//     Usart2FillRxBuffer("temp,055.aa\r\n");
//     Comms_Channel1 ();
    
//     printf("Test antenna error on temp decimal: ");
//     if (cb_usart_value == 0)
//         PrintOK();
//     else
//         PrintERR();

//     global_ant.resistance_int = 0;
//     global_ant.resistance_dec = 0;
//     global_ant.inductance_int = 0;
//     global_ant.inductance_dec = 0;
        
//     strcpy (my_ant_str2,"ant3,003.50,019.00,003.50,065.50\r\n");
//     cb_usart_value = 0;    
//     Usart2FillRxBuffer(my_ant_str2);
//     Comms_Channel1 ();
    
//     printf("Test new antenna with good params: ");
//     if ((cb_usart_value == 3) &&
//         (global_ant.resistance_int == 3) &&
//         (global_ant.resistance_dec == 50) &&
//         (global_ant.inductance_int == 19) &&
//         (global_ant.inductance_dec == 0) &&
//         (global_ant.current_limit_int == 3) &&
//         (global_ant.current_limit_dec == 50) &&
//         (global_ant.temp_max_int == 65) &&
//         (global_ant.temp_max_dec == 50))
//         PrintOK();
//     else
//         PrintERR();
    
// }


// Module Mocked Functions -----------------------------------------------------
void Tim_Bit_Bang_Tx_Reset(void)
{
    printf("tim int reset\n");
}


void Tim_Bit_Bang_Tx_Set(void)
{
    printf("tim int set\n");
}


void Tx_Pin_On (void)
{
    printf(" 1");
}


void Tx_Pin_Off (void)
{
    printf(" 0");
}



//--- end of file ---//


