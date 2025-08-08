//---------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### COMMS.C ###########################################
//---------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "comms.h"
// #include "answers_defs.h"
#include "hard.h"
#include "supply.h"
#include "usart.h"


#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// Module Private Types Constants and Macros -----------------------------------
char s_ans_ok [] = {"ok\n"};
char s_ans_nok [] = {"nok\n"};
#define SIZEOF_COMMS_BUFF    128
#define COMMS_TT_RELOAD    3000


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
char comms_buff [SIZEOF_COMMS_BUFF];
    

// Module Private Functions ----------------------------------------------------
static void Comms_Messages (char * msg_str);


// Module Functions ------------------------------------------------------------
void Comms_Update (void)
{
    if (Usart3HaveData())
    {
        Usart3HaveDataReset();
        Usart3ReadBuffer(comms_buff, SIZEOF_COMMS_BUFF);
        Comms_Messages(comms_buff);
    }
}


static void Comms_Messages (char * msg_str)
{
    char buff [128];    
    
    if (strncmp (msg_str, "get volts", sizeof("get volts") - 1) == 0)
    {
	Supply_Send_Voltage_Start ();
    }

    else if (strncmp (msg_str, "get batt", sizeof("get batt") - 1) == 0)
    {
	Supply_Send_Voltage_Start ();
    }

    else if (strncmp (msg_str, "get adc", sizeof("get adc") - 1) == 0)
    {
	sprintf(buff, "vin: %d bst: %d b1: %d b2: %d b3: %d b4: %d\r\n",
		Supply_Get_Mains(),
		Supply_Get_Boost(),
		Supply_Get_Batt(0),
		Supply_Get_Batt(1),
		Supply_Get_Batt(2),
		Supply_Get_Batt(3));

	Usart3Send(buff);
    }
    
    else if (strncmp (msg_str, "get mode", sizeof("get mode") - 1) == 0)
    {
	unsigned char mode = 0;

	mode = Supply_Get_Mode();

	if (mode == SUPPLY_MODE_UNKNOW)
	{
	    Usart3Send("supply mode unknow\r\n");
	}
	else if (mode == SUPPLY_MODE_BATT)
	{
	    Usart3Send("supply mode battery\r\n");
	}
	else    // SUPPLY_MODE_MAINS
	{
	    Usart3Send("supply mode mains\r\n");
	}
    }

    // else
    //     Usart3Send(s_ans_nok);

}


//---- End of File ----//
