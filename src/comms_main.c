//---------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### COMMS_MAIN.C ######################################
//---------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "comms_main.h"
// #include "answers_defs.h"
#include "hard.h"
#include "usart_channels.h"


#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// Module Private Types Constants and Macros -----------------------------------
char s_ans_ok [] = {"ok\n"};
char s_ans_nok [] = {"nok\n"};
#define SIZEOF_COMMS_BUFF    128


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
char comms_buff [SIZEOF_COMMS_BUFF];


// Module Private Functions ----------------------------------------------------
static void Comms_Main_Messages (char * msg_str);


// Module Functions ------------------------------------------------------------
void Comms_Main_Update (void)
{
    if (UsartMainHaveData ())
    {
        UsartMainHaveDataReset ();
        UsartMainReadBuffer (comms_buff, SIZEOF_COMMS_BUFF);
        Comms_Main_Messages (comms_buff);
    }
}


static void Comms_Main_Messages (char * msg_str)
{
    // char buff [128];    

    // "enc 0 1\n"
    if (strncmp (msg_str, "enc ", sizeof("enc ") - 1) == 0)
    {
	char * pmsg = msg_str + sizeof("enc ") - 1;

	// check enable if valid number
	if ((*pmsg >= '0') && (*pmsg <= '7'))
	{
	    if ((*(pmsg + 1) == ' ') &&
		(*(pmsg + 2) >= '0') &&
		(*(pmsg + 2) >= ';'))
	    {
		unsigned char encoder;
		unsigned char position = 0;

		// encoder from 0 to 7, position from 0 to 10
		encoder = *(pmsg + 0) - '0';
		position = *(pmsg + 2) - '0';
		Encoder_Buffer_Update (encoder, position);
	    }
		
	}
    }

    else if (strncmp (msg_str, "rpi is up", sizeof("rpi is up") - 1) == 0)
    {
	UsartMainSend (s_ans_ok);	
    }

    else if (strncmp (msg_str, "rpi is down", sizeof("rpi is down") - 1) == 0)
    {
	UsartMainSend (s_ans_ok);
    }
    
    // else
    //     Usart3Send(s_ans_nok);

}


//---- End of File ----//
