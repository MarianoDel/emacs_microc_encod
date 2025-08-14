//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### ENCODERS_MANAGER.C ####################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "encoders_manager.h"
#include "encoder_utils.h"
#include "usart_channels.h"


#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    MNGR_INIT,
    MNGR_POWERUP,
    MNGR_POWERUP_BLANK,    
    MNGR_RUNNING,
    MNGR_SHUTTINGDOWN
    
} manager_states_e;


// Module Private Functions ----------------------------------------------------
void Encoders_Manager_PowerUp_Reset (void);
void Encoders_Manager_PowerUp (void);

void Encoders_Manager_ShuttingDown_Reset (void);
void Encoders_Manager_ShuttingDown (void);

void Encoders_Manager_Check_Encoders (void);
void Encoders_Manager_Check_Enc1 (unsigned char * waiting_a_read);
void Encoders_Manager_Check_Enc2 (unsigned char * waiting_a_read);
void Encoders_Manager_Check_Enc3 (unsigned char * waiting_a_read);
void Encoders_Manager_Check_Enc4 (unsigned char * waiting_a_read);


// Module Globals --------------------------------------------------------------
volatile unsigned short encoders_timeout = 0;
unsigned char encoders_up_down = 0;
manager_states_e manager_state = MNGR_INIT;
unsigned char encoders_from_rpi [4] = { 0 };
unsigned char encoders_from_rpi_last [4] = { 0 };
unsigned char encoders_wait_read [4] = { 0 };


// Module Functions ------------------------------------------------------------
void Encoders_Manager_Timeouts (void)
{
    if (encoders_timeout)
	encoders_timeout--;
    
	
}


void Encoders_Manager_Update (void)
{
    switch (manager_state)
    {
    case MNGR_INIT:
	Encoders_Manager_PowerUp_Reset ();
	manager_state++;
	break;

    case MNGR_POWERUP:
	Encoders_Manager_PowerUp ();
	
	if (Encoders_Manager_Rpi_Get())
	{
	    // blank 1 and 4
	    Encoder_Write_Address_Plain (ENCOD_1, 0x7000, 0x0002);
	    Encoder_Write_Address_Plain (ENCOD_2, 0x7000, 0x0002);
	    Encoder_Write_Address_Plain (ENCOD_3, 0x7000, 0x0002);
	    Encoder_Write_Address_Plain (ENCOD_4, 0x7000, 0x0002);

	    // save init config
	    for (int i = ENCOD_1; i <= ENCOD_4; i++)
	    {
		encoders_from_rpi[i] = 0;
		encoders_from_rpi_last[i] = 0;		
	    }

	    encoders_timeout = 200;
	    manager_state++;
	}
	break;

    case MNGR_POWERUP_BLANK:
	if (encoders_timeout)
	    break;
	
	manager_state++;
	break;
	
    case MNGR_RUNNING:
	Encoders_Manager_Check_Encoders ();

	if (!Encoders_Manager_Rpi_Get ())
	{
	    Encoders_Manager_ShuttingDown_Reset ();
	    encoders_timeout = 0;
	    manager_state++;
	}
	break;

    case MNGR_SHUTTINGDOWN:
	Encoders_Manager_ShuttingDown ();
	
	break;

    default:
	manager_state = MNGR_INIT;
	break;
    }
}


unsigned char encoders_manager_rpi_is_up = 0;
unsigned char Encoders_Manager_Rpi_Get (void)
{
    return encoders_manager_rpi_is_up;
}


void Encoders_Manager_Rpi_Set (unsigned char new_rpi_state)
{
    encoders_manager_rpi_is_up = new_rpi_state;
}



void Encoders_Manager_PowerUp_Reset (void)
{
    encoders_up_down = 0;
}


void Encoders_Manager_PowerUp (void)
{
    if (encoders_timeout)
	return;

    encoders_timeout = 1000;
    
    if (encoders_up_down == 0)
    {
	// powerup 1 to 4
	Encoder_Write_Address_Plain (ENCOD_1, 0x7000, 0x0010);
	Encoder_Write_Address_Plain (ENCOD_2, 0x7000, 0x000B);
	Encoder_Write_Address_Plain (ENCOD_3, 0x7000, 0x0010);
	Encoder_Write_Address_Plain (ENCOD_4, 0x7000, 0x000B);
	encoders_up_down = 1;
    }
    else if (encoders_up_down == 1)
    {
	// blank 1 and 4
	Encoder_Write_Address_Plain (ENCOD_1, 0x7000, 0x000F);
	Encoder_Write_Address_Plain (ENCOD_2, 0x7000, 0x000A);
	Encoder_Write_Address_Plain (ENCOD_3, 0x7000, 0x000F);
	Encoder_Write_Address_Plain (ENCOD_4, 0x7000, 0x000A);
	encoders_up_down = 0;
    }	
}


void Encoders_Manager_ShuttingDown_Reset (void)
{
    encoders_up_down = 0;
}


void Encoders_Manager_ShuttingDown (void)
{
    if (encoders_timeout)
	return;

    encoders_timeout = 1000;
    
    if (encoders_up_down == 0)
    {
	// shutting down 1 and 4
	Encoder_Write_Address_Plain (ENCOD_1, 0x7000, 0x0011);
	Encoder_Write_Address_Plain (ENCOD_2, 0x7000, 0x000C);
	Encoder_Write_Address_Plain (ENCOD_3, 0x7000, 0x0011);
	Encoder_Write_Address_Plain (ENCOD_4, 0x7000, 0x000C);
	encoders_up_down = 1;
    }
    else if (encoders_up_down == 1)
    {
	// blank 1 and 4
	Encoder_Write_Address_Plain (ENCOD_1, 0x7000, 0x000F);
	Encoder_Write_Address_Plain (ENCOD_2, 0x7000, 0x000A);
	Encoder_Write_Address_Plain (ENCOD_3, 0x7000, 0x000F);
	Encoder_Write_Address_Plain (ENCOD_4, 0x7000, 0x000A);
	encoders_up_down = 0;
    }	
}


void Encoders_Manager_Check_Encoders (void)
{
    if (!encoders_timeout)
    {
	encoders_timeout = 50;

	// ENCOD_1 update from rpi or read status
	if (encoders_from_rpi_last[ENCOD_1] != encoders_from_rpi[ENCOD_1])
	{
	    // update encoder from rpi
	    encoders_from_rpi_last[ENCOD_1] = encoders_from_rpi[ENCOD_1];
	    Encoder_Write_Address_Plain (ENCOD_1,
					 0x7000,
					 0x0002 + encoders_from_rpi_last[ENCOD_1]);
	}
	else
	{
	    // read encoder status
	    Encoder_Read_Address_Plain (ENCOD_1, 0x7000, 0x0001);
	    encoders_wait_read[ENCOD_1] = 1;
	}

	// ENCOD_2 update from rpi or read status
	if (encoders_from_rpi_last[ENCOD_2] != encoders_from_rpi[ENCOD_2])
	{
	    // update encoder from rpi
	    encoders_from_rpi_last[ENCOD_2] = encoders_from_rpi[ENCOD_2];
	    Encoder_Write_Address_Plain (ENCOD_2,
					 0x7000,
					 0x0002 + encoders_from_rpi_last[ENCOD_2]);
	}
	else
	{
	    // read encoder status
	    Encoder_Read_Address_Plain (ENCOD_2, 0x7000, 0x0001);
	    encoders_wait_read[ENCOD_2] = 1;
	}

	// ENCOD_3 update from rpi or read status
	if (encoders_from_rpi_last[ENCOD_3] != encoders_from_rpi[ENCOD_3])
	{
	    // update encoder from rpi
	    encoders_from_rpi_last[ENCOD_3] = encoders_from_rpi[ENCOD_3];
	    Encoder_Write_Address_Plain (ENCOD_3,
					 0x7000,
					 0x0002 + encoders_from_rpi_last[ENCOD_3]);
	}
	else
	{
	    // read encoder status
	    Encoder_Read_Address_Plain (ENCOD_3, 0x7000, 0x0001);
	    encoders_wait_read[ENCOD_3] = 1;
	}

	// ENCOD_4 update from rpi or read status
	if (encoders_from_rpi_last[ENCOD_4] != encoders_from_rpi[ENCOD_4])
	{
	    // update encoder from rpi
	    encoders_from_rpi_last[ENCOD_4] = encoders_from_rpi[ENCOD_4];
	    Encoder_Write_Address_Plain (ENCOD_4,
					 0x7000,
					 0x0002 + encoders_from_rpi_last[ENCOD_4]);
	}
	else
	{
	    // read encoder status
	    Encoder_Read_Address_Plain (ENCOD_4, 0x7000, 0x0001);
	    encoders_wait_read[ENCOD_4] = 1;
	}
    }
    
    Encoders_Manager_Check_Enc1 (&encoders_wait_read[ENCOD_1]);
    Encoders_Manager_Check_Enc2 (&encoders_wait_read[ENCOD_2]);
    Encoders_Manager_Check_Enc3 (&encoders_wait_read[ENCOD_3]);
    Encoders_Manager_Check_Enc4 (&encoders_wait_read[ENCOD_4]);
        
}


void Encoders_Manager_Check_Enc1 (unsigned char * waiting_a_read)
{
    if (!(UsartEncoder1HaveData()))
	return;
    
    int bytes = 0;
    unsigned char bbuff [100];
    unsigned char answer = BUF_UNKNOW;
    
    UsartEncoder1HaveDataReset();
    bytes = UsartEncoder1ReadBuffer((char *) bbuff, 100);
    answer = Encoder_Process_Buffer_Plain (bytes, bbuff);

    if (answer != BUF_READ_CRC_OK_LEN_OK)
	return;
	
    if (!(*waiting_a_read))
	return;
	
    int base = 15;
    unsigned short val = 0;
    val = bbuff[base + 0];
    val <<= 8;
    val |= bbuff[base + 1];

    // check valid values
    if ((val == 0) ||
	(val == 1))
    {
	// page 0 or 1 goto 2
	Encoder_Write_Address_Plain (ENCOD_1, 0x7000, 0x0002);
	val = 2;
	// UsartMainSend("\r\npage 2\r\n");
    }
		    
    if ((val == 13) ||
	(val == 14))
    {
	// page 13 or 14 goto 12
	Encoder_Write_Address_Plain (ENCOD_1, 0x7000, 0x000C);
	val = 12;
	// UsartMainSend("\r\npage 12\r\n");
    }

    if ((val - 2) != encoders_from_rpi_last[ENCOD_1])
    {
	encoders_from_rpi[ENCOD_1] = val - 2;
	encoders_from_rpi_last[ENCOD_1] = val - 2;
	char buf [30] = { 0 };
	sprintf(buf, "encp 0 %c\r\n", (val - 2) + '0');
	UsartMainSend(buf);	
    }
    
    *waiting_a_read = 0;
}


void Encoders_Manager_Check_Enc2 (unsigned char * waiting_a_read)
{
    if (!(UsartEncoder2HaveData()))
	return;
    
    int bytes = 0;
    unsigned char bbuff [100];
    unsigned char answer = BUF_UNKNOW;

    UsartEncoder2HaveDataReset();
    bytes = UsartEncoder2ReadBuffer((char *) bbuff, 100);
    answer = Encoder_Process_Buffer_Plain (bytes, bbuff);

    if (answer != BUF_READ_CRC_OK_LEN_OK)
	return;
	
    if (!(*waiting_a_read))
	return;
	
    int base = 15;
    unsigned short val = 0;
    val = bbuff[base + 0];
    val <<= 8;
    val |= bbuff[base + 1];

    if ((val == 0) ||
	(val == 1))
    {
	// page 0 or 1 goto 2
	Encoder_Write_Address_Plain (ENCOD_2, 0x7000, 0x0002);
	val = 2;
	// UsartMainSend("\r\npage 2\r\n");
    }
		    
    if ((val == 8) ||
	(val == 9))
    {
	// page 8 or 9 goto 7
	Encoder_Write_Address_Plain (ENCOD_2, 0x7000, 0x0007);
	val = 7;
	// UsartMainSend("\r\npage 7\r\n");
    }

    if ((val - 2) != encoders_from_rpi_last[ENCOD_2])
    {
	encoders_from_rpi[ENCOD_2] = val - 2;
	encoders_from_rpi_last[ENCOD_2] = val - 2;
	char buf [30] = { 0 };
	sprintf(buf, "encp 1 %c\r\n", (val - 2) + '0');
	UsartMainSend(buf);	
    }
    
    *waiting_a_read = 0;
}


void Encoders_Manager_Check_Enc3 (unsigned char * waiting_a_read)
{
    if (!(UsartEncoder3HaveData()))
	return;
    
    int bytes = 0;
    unsigned char bbuff [100];    
    unsigned char answer = BUF_UNKNOW;
    
    UsartEncoder3HaveDataReset();
    bytes = UsartEncoder3ReadBuffer((char *) bbuff, 100);
    answer = Encoder_Process_Buffer_Plain (bytes, bbuff);

    if (answer != BUF_READ_CRC_OK_LEN_OK)
	return;
	
    if (!(*waiting_a_read))
	return;
	
    int base = 15;
    unsigned short val = 0;
    val = bbuff[base + 0];
    val <<= 8;
    val |= bbuff[base + 1];

    if ((val == 0) ||
	(val == 1))
    {
	// page 0 or 1 goto 2
	Encoder_Write_Address_Plain (ENCOD_3, 0x7000, 0x0002);
	val = 2;
	// UsartMainSend("\r\npage 2\r\n");
    }
		    
    if ((val == 13) ||
	(val == 14))
    {
	// page 13 or 14 goto 12
	Encoder_Write_Address_Plain (ENCOD_3, 0x7000, 0x000C);
	val = 12;
	// UsartMainSend("\r\npage 12\r\n");
    }

    if ((val - 2) != encoders_from_rpi_last[ENCOD_3])
    {
	encoders_from_rpi[ENCOD_3] = val - 2;
	encoders_from_rpi_last[ENCOD_3] = val - 2;
	char buf [30] = { 0 };
	sprintf(buf, "encp 2 %c\r\n", (val - 2) + '0');
	UsartMainSend(buf);	
    }
    
    *waiting_a_read = 0;
}


void Encoders_Manager_Check_Enc4 (unsigned char * waiting_a_read)
{
    if (!(UsartEncoder4HaveData()))
	return;
    
    int bytes = 0;
    unsigned char bbuff [100];
    unsigned char answer = BUF_UNKNOW;

    UsartEncoder4HaveDataReset();
    bytes = UsartEncoder4ReadBuffer((char *) bbuff, 100);
    answer = Encoder_Process_Buffer_Plain (bytes, bbuff);

    if (answer != BUF_READ_CRC_OK_LEN_OK)
	return;
	
    if (!(*waiting_a_read))
	return;
	
    int base = 15;
    unsigned short val = 0;
    val = bbuff[base + 0];
    val <<= 8;
    val |= bbuff[base + 1];

    if ((val == 0) ||
	(val == 1))
    {
	// page 0 or 1 goto 2
	Encoder_Write_Address_Plain (ENCOD_4, 0x7000, 0x0002);
	val = 2;
	// UsartMainSend("\r\npage 2\r\n");
    }
		    
    if ((val == 8) ||
	(val == 9))
    {
	// page 8 or 9 goto 7
	Encoder_Write_Address_Plain (ENCOD_4, 0x7000, 0x0007);
	val = 7;
	// UsartMainSend("\r\npage 7\r\n");
    }

    if ((val - 2) != encoders_from_rpi_last[ENCOD_4])
    {
	encoders_from_rpi[ENCOD_4] = val - 2;
	encoders_from_rpi_last[ENCOD_4] = val - 2;
	char buf [30] = { 0 };
	sprintf(buf, "encp 3 %c\r\n", (val - 2) + '0');
	UsartMainSend(buf);	
    }
    
    *waiting_a_read = 0;
}


void Encoders_Manager_Buffer_Update (unsigned char encoder,
				     unsigned char new_position)
{
    if (encoder > ENCOD_4)
	return;
    
    encoders_from_rpi [encoder] = new_position;
}


//--- end of file ---//
