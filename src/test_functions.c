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
#include "usart_channels.h"
#include "tim.h"
#include "encoder_utils.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------
extern volatile unsigned short wait_ms_var;
extern volatile unsigned short timer_standby;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
void TF_UsartMain_Tx (void);
void TF_UsartMain_Loop (void);
void TF_UsartMain_Encoder (unsigned char encoder_number);
void TF_UsartMain_Encoder1 (void);
void TF_UsartMain_Encoder2 (void);
void TF_UsartMain_Encoder3 (void);
void TF_UsartMain_Encoder4 (void);


// Module Functions ------------------------------------------------------------
void TF_Hardware_Tests (void)
{
    // TF_UsartMain_Tx ();
    // TF_UsartMain_Loop ();
    // TF_UsartMain_Encoder1 ();
    TF_UsartMain_Encoder4 ();    

}


void TF_UsartMain_Tx (void)
{
    UsartMainConfig ();
    
    while (1)
    {
        if (!timer_standby)
        {
            UsartMainSend ("Mariano\r\n");
            timer_standby = 2000;
        }
    }
}


void TF_UsartMain_Loop (void)
{
    char buff [100];
    char sbuff [200];
    int send_buffer = 0;
    int sendlen = 0;
    int getlen = 0;
    
    UsartMainConfig ();
    UsartMainSend("\r\nLoop test on UsartMain\r\n");
    
    while (1)
    {
        if (UsartMainHaveData ())
        {
	    memset(buff, '\0', 100);
            UsartMainHaveDataReset();
            getlen = UsartMainReadBuffer(buff, 100);
	    send_buffer = 1;
	    timer_standby = 2000;
        }

	if ((send_buffer) && (!timer_standby))
	{
	    send_buffer = 0;
	    sendlen = strlen (buff);
	    sprintf(sbuff, "get %d send %d bytes: %s\r\n", getlen, sendlen, buff);
	    UsartMainSend(sbuff);
	}
    }
}


void TF_UsartMain_Encoder (unsigned char encoder_number)
{
    char buff [100];
    unsigned char bbuff [100];    
    unsigned short addr = 0;
    unsigned short bytes_or_value = 0;
    
    UsartMainConfig ();

    switch (encoder_number)
    {
    case ENCOD_1:
	UsartEncoder1Config ();
	break;
    case ENCOD_2:
	UsartEncoder2Config ();	
	break;
    case ENCOD_3:
	UsartEncoder3Config ();	
	break;
    case ENCOD_4:
	UsartEncoder4Config ();	
	break;
    }

    sprintf(buff, "\r\n-- USING ENCODER: %d --\r\n", encoder_number + 1);
    UsartMainSend(buff);
    
    UsartMainSend("\r\nWrite (w) to or Read (r) from Address\r\n");

    int state = 0;
    while (1)
    {
	if (state == 0)
	{
	    if (UsartMainHaveData ())
	    {
		memset(buff, '\0', 100);
		UsartMainHaveDataReset();
		UsartMainReadBuffer(buff, 100);

		if (buff[0] == 'r')
		{
		    state = 1;
		    UsartMainSend("Address to read (4 chars) in hex: ");
		    timer_standby = 20000;
		}
		else if (buff[0] == 'w')
		{
		    state = 10;
		    UsartMainSend("Address to write (4 chars) in hex: ");
		    timer_standby = 20000;		    
		}
		else
		{
		    UsartMainSend("error!\r\n");
		    UsartMainSend("\r\nWrite (w) to or Read (r) from Address\r\n");
		    state = 0;
		}
	    }
	}

	// states for read
	if (state == 1)
	{
	    if (UsartMainHaveData ())
	    {
		int b = 0;
		memset(buff, '\0', 100);
		UsartMainHaveDataReset();
		UsartMainReadBuffer(buff, 100);
		b = StringIsANumberHex (buff, &addr);

		if (b == 4)
		{
		    UsartMainSend("\r\nHow many bytes (4 chars) in hex: ");
		    state = 2;
		}
		else
		{
		    UsartMainSend("error!\r\n");
		    state = 30;
		}
	    }
	    
	    if (!timer_standby)
		state = 30;
	}

	if (state == 2)
	{
	    if (UsartMainHaveData ())
	    {
		int b = 0;
		memset(buff, '\0', 100);
		UsartMainHaveDataReset();
		UsartMainReadBuffer(buff, 100);
		b = StringIsANumberHex (buff, &bytes_or_value);

		if (b == 4)
		{
		    UsartMainSend("\r\n");
		    // Encoder_Read_Address(ENCOD_1, addr, bytes_or_value);
		    Encoder_Read_Address(encoder_number, addr, bytes_or_value);		    
		    timer_standby = 5000;
		    state = 30;
		}
		else
		{
		    UsartMainSend("error!\r\n");
		    state = 30;
		}
	    }
	    
	    if (!timer_standby)
		state = 30;
	}

	// states for write
	if (state == 10)
	{
	    if (UsartMainHaveData ())
	    {
		int b = 0;
		memset(buff, '\0', 100);
		UsartMainHaveDataReset();
		UsartMainReadBuffer(buff, 100);
		b = StringIsANumberHex (buff, &addr);

		if (b == 4)
		{
		    UsartMainSend("\r\nNew value (4 chars) in hex: ");
		    state = 11;
		}
		else
		{
		    UsartMainSend("error!\r\n");
		    state = 30;
		}
	    }
	    
	    if (!timer_standby)
		state = 30;
	}

	if (state == 11)
	{
	    if (UsartMainHaveData ())
	    {
		int b = 0;
		memset(buff, '\0', 100);
		UsartMainHaveDataReset();
		UsartMainReadBuffer(buff, 100);
		b = StringIsANumberHex (buff, &bytes_or_value);

		if (b == 4)
		{
		    UsartMainSend("\r\n");		    
		    // Encoder_Write_Address(ENCOD_1, addr, bytes_or_value);
		    Encoder_Write_Address(encoder_number, addr, bytes_or_value);		    
		    timer_standby = 5000;
		    state = 30;
		}
		else
		{
		    UsartMainSend("error!\r\n");
		    state = 30;
		}
	    }

	    if (!timer_standby)
		state = 30;
	}


	// state for loop again
	if ((state == 30) && (!timer_standby))
	{
	    UsartMainSend("\r\nWrite (w) to or Read (r) from Address\r\n");
	    state = 0;
	}

	// always check rx
	switch (encoder_number)
	{
	case ENCOD_1:
	    if (UsartEncoder1HaveData())
	    {
		int bytes = 0;
		UsartEncoder1HaveDataReset();
		bytes = UsartEncoder1ReadBuffer((char *) bbuff, 100);

		sprintf(buff, "\r\nget: %d bytes\r\n", bytes);
		UsartMainSend(buff);
		
		Encoder_Process_Buffer(bytes, bbuff);
	    }
	    break;
	case ENCOD_2:
	    if (UsartEncoder2HaveData())
	    {
		int bytes = 0;
		UsartEncoder2HaveDataReset();
		bytes = UsartEncoder2ReadBuffer((char *) bbuff, 100);

		sprintf(buff, "\r\nget: %d bytes\r\n", bytes);
		UsartMainSend(buff);
		
		Encoder_Process_Buffer(bytes, bbuff);
	    }	
	    break;
	case ENCOD_3:
	    if (UsartEncoder3HaveData())
	    {
		int bytes = 0;
		UsartEncoder3HaveDataReset();
		bytes = UsartEncoder3ReadBuffer((char *) bbuff, 100);

		sprintf(buff, "\r\nget: %d bytes\r\n", bytes);
		UsartMainSend(buff);
		
		Encoder_Process_Buffer(bytes, bbuff);
	    }	
	    break;
	case ENCOD_4:
	    if (UsartEncoder4HaveData())
	    {
		int bytes = 0;
		UsartEncoder4HaveDataReset();
		bytes = UsartEncoder4ReadBuffer((char *) bbuff, 100);

		sprintf(buff, "\r\nget: %d bytes\r\n", bytes);
		UsartMainSend(buff);
		
		Encoder_Process_Buffer(bytes, bbuff);
	    }	
	    break;
	}
    }
}


void TF_UsartMain_Encoder1 (void)
{
    TF_UsartMain_Encoder (ENCOD_1);
}


void TF_UsartMain_Encoder2 (void)
{
    TF_UsartMain_Encoder (ENCOD_2);
}


void TF_UsartMain_Encoder3 (void)
{
    TF_UsartMain_Encoder (ENCOD_3);
}


void TF_UsartMain_Encoder4 (void)
{
    TF_UsartMain_Encoder (ENCOD_4);
}



//--- end of file ---//
