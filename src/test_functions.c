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
void TF_UsartMain_Encoder1_Top_And_Bottom (void);
void TF_UsartMain_Encoder4_Top_And_Bottom (void);
void TF_UsartMain_Encoder_1_4_Top_And_Bottom (void);
void TF_UsartMain_Encoder_1_2_3_4_PowerUp_ShuttingDown (void);


// Module Functions ------------------------------------------------------------
void TF_Hardware_Tests (void)
{
    // TF_UsartMain_Tx ();
    // TF_UsartMain_Loop ();
    // TF_UsartMain_Encoder1 ();
    // TF_UsartMain_Encoder4 ();
    // TF_UsartMain_Encoder1_Top_And_Bottom ();
    // TF_UsartMain_Encoder4_Top_And_Bottom ();

    // TF_UsartMain_Encoder_1_4_Top_And_Bottom ();
    TF_UsartMain_Encoder_1_2_3_4_PowerUp_ShuttingDown ();    

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


void TF_UsartMain_Encoder4_Top_And_Bottom (void)
{
    unsigned char bbuff [100];    
    
    UsartMainConfig ();

    UsartEncoder4Config ();	

    UsartMainSend("\r\n-- USING ENCODER: 4 Top and Bottom --\r\n");
    
    unsigned short value_to_read = 0;
    while (1)
    {
	if (!timer_standby)
	{
	    timer_standby = 50;

	    Encoder_Read_Address_Plain (ENCOD_4, 0x7000, 0x0001);
	    value_to_read = 1;
	    // UsartMainSend(".");
	}

	if (UsartEncoder4HaveData())
	{
	    int bytes = 0;
	    unsigned char answer = BUF_UNKNOW;
	    UsartEncoder4HaveDataReset();
	    bytes = UsartEncoder4ReadBuffer((char *) bbuff, 100);

	    // sprintf(buff, "\r\nget: %d bytes\r\n", bytes);
	    // UsartMainSend(buff);	
	    // Encoder_Process_Buffer(bytes, bbuff);

	    answer = Encoder_Process_Buffer_Plain (bytes, bbuff);
	    // sprintf(buff, "\r\n answer: %d\r\n", answer);
	    // UsartMainSend(buff);	

	    if (answer == BUF_READ_CRC_OK_LEN_OK)
	    {
		if (value_to_read == 1)
		{
		    value_to_read = 0;

		    int base = 15;
		    unsigned short val = 0;
		    val = bbuff[base + 0];
		    val <<= 8;
		    val |= bbuff[base + 1];

		    // sprintf(buff, "\r\n value: %d\r\n", val);
		    // UsartMainSend(buff);	
		    
		    if ((val == 0) ||
			(val == 1))
		    {
			// page 0 or 1 goto 2
			Encoder_Write_Address_Plain (ENCOD_4, 0x7000, 0x0002);
			UsartMainSend("\r\npage 2\r\n");
		    }
		    
		    if ((val == 8) ||
			(val == 9))
		    {
			// page 8 or 9 goto 7
			Encoder_Write_Address_Plain (ENCOD_4, 0x7000, 0x0007);
			UsartMainSend("\r\npage 7\r\n");
		    }
		}
	    }
	}	
    }
}


void TF_UsartMain_Encoder1_Top_And_Bottom (void)
{
    unsigned char bbuff [100];    
    
    UsartMainConfig ();

    UsartEncoder1Config ();	

    UsartMainSend("\r\n-- USING ENCODER: 1 Top and Bottom --\r\n");
    
    unsigned short value_to_read = 0;
    while (1)
    {
	if (!timer_standby)
	{
	    timer_standby = 12;

	    Encoder_Read_Address_Plain (ENCOD_1, 0x7000, 0x0001);
	    value_to_read = 1;
	    // UsartMainSend(".");
	}

	if (UsartEncoder1HaveData())
	{
	    int bytes = 0;
	    unsigned char answer = BUF_UNKNOW;
	    UsartEncoder1HaveDataReset();
	    bytes = UsartEncoder1ReadBuffer((char *) bbuff, 100);

	    // sprintf(buff, "\r\nget: %d bytes\r\n", bytes);
	    // UsartMainSend(buff);	
	    // Encoder_Process_Buffer(bytes, bbuff);

	    answer = Encoder_Process_Buffer_Plain (bytes, bbuff);
	    // sprintf(buff, "\r\n answer: %d\r\n", answer);
	    // UsartMainSend(buff);	

	    if (answer == BUF_READ_CRC_OK_LEN_OK)
	    {
		if (value_to_read == 1)
		{
		    value_to_read = 0;

		    int base = 15;
		    unsigned short val = 0;
		    val = bbuff[base + 0];
		    val <<= 8;
		    val |= bbuff[base + 1];

		    // sprintf(buff, "\r\n value: %d\r\n", val);
		    // UsartMainSend(buff);	
		    
		    if ((val == 0) ||
			(val == 1))
		    {
			// page 0 or 1 goto 2
			Encoder_Write_Address_Plain (ENCOD_1, 0x7000, 0x0002);
			UsartMainSend("\r\npage 2\r\n");
		    }
		    
		    if ((val == 13) ||
			(val == 14))
		    {
			// page 13 or 14 goto 12
			Encoder_Write_Address_Plain (ENCOD_1, 0x7000, 0x000C);
			UsartMainSend("\r\npage 12\r\n");
		    }
		}
	    }
	}	
    }
}


void TF_UsartMain_Encoder_1_4_Top_And_Bottom (void)
{
    unsigned char bbuff [100];    
    
    UsartMainConfig ();

    UsartEncoder1Config ();
    UsartEncoder4Config ();

    UsartMainSend("\r\n-- USING ENCODER: 1 and 4 Top and Bottom --\r\n");
    
    unsigned short value_to_read1 = 0;
    unsigned short value_to_read4 = 0;    
    while (1)
    {
	if (!timer_standby)
	{
	    timer_standby = 50;

	    Encoder_Read_Address_Plain (ENCOD_1, 0x7000, 0x0001);
	    value_to_read1 = 1;
	    // UsartMainSend(".");

	    Encoder_Read_Address_Plain (ENCOD_4, 0x7000, 0x0001);
	    value_to_read4 = 1;
	    
	}

	if (UsartEncoder1HaveData())
	{
	    int bytes = 0;
	    unsigned char answer = BUF_UNKNOW;
	    UsartEncoder1HaveDataReset();
	    bytes = UsartEncoder1ReadBuffer((char *) bbuff, 100);

	    // sprintf(buff, "\r\nget: %d bytes\r\n", bytes);
	    // UsartMainSend(buff);	
	    // Encoder_Process_Buffer(bytes, bbuff);

	    answer = Encoder_Process_Buffer_Plain (bytes, bbuff);
	    // sprintf(buff, "\r\n answer: %d\r\n", answer);
	    // UsartMainSend(buff);	

	    if (answer == BUF_READ_CRC_OK_LEN_OK)
	    {
		if (value_to_read1 == 1)
		{
		    value_to_read1 = 0;

		    int base = 15;
		    unsigned short val = 0;
		    val = bbuff[base + 0];
		    val <<= 8;
		    val |= bbuff[base + 1];

		    // sprintf(buff, "\r\n value: %d\r\n", val);
		    // UsartMainSend(buff);	
		    
		    if ((val == 0) ||
			(val == 1))
		    {
			// page 0 or 1 goto 2
			Encoder_Write_Address_Plain (ENCOD_1, 0x7000, 0x0002);
			UsartMainSend("\r\npage 2\r\n");
		    }
		    
		    if ((val == 13) ||
			(val == 14))
		    {
			// page 13 or 14 goto 12
			Encoder_Write_Address_Plain (ENCOD_1, 0x7000, 0x000C);
			UsartMainSend("\r\npage 12\r\n");
		    }
		}
	    }
	}

	if (UsartEncoder4HaveData())
	{
	    int bytes = 0;
	    unsigned char answer = BUF_UNKNOW;
	    UsartEncoder4HaveDataReset();
	    bytes = UsartEncoder4ReadBuffer((char *) bbuff, 100);

	    // sprintf(buff, "\r\nget: %d bytes\r\n", bytes);
	    // UsartMainSend(buff);	
	    // Encoder_Process_Buffer(bytes, bbuff);

	    answer = Encoder_Process_Buffer_Plain (bytes, bbuff);
	    // sprintf(buff, "\r\n answer: %d\r\n", answer);
	    // UsartMainSend(buff);	

	    if (answer == BUF_READ_CRC_OK_LEN_OK)
	    {
		if (value_to_read4 == 1)
		{
		    value_to_read4 = 0;

		    int base = 15;
		    unsigned short val = 0;
		    val = bbuff[base + 0];
		    val <<= 8;
		    val |= bbuff[base + 1];

		    // sprintf(buff, "\r\n value: %d\r\n", val);
		    // UsartMainSend(buff);	
		    
		    if ((val == 0) ||
			(val == 1))
		    {
			// page 0 or 1 goto 2
			Encoder_Write_Address_Plain (ENCOD_4, 0x7000, 0x0002);
			UsartMainSend("\r\npage 2\r\n");
		    }
		    
		    if ((val == 8) ||
			(val == 9))
		    {
			// page 8 or 9 goto 7
			Encoder_Write_Address_Plain (ENCOD_4, 0x7000, 0x0007);
			UsartMainSend("\r\npage 7\r\n");
		    }
		}
	    }
	}	
    }
}


void TF_UsartMain_Encoder_1_2_3_4_PowerUp_ShuttingDown (void)
{
    UsartMainConfig ();

    UsartEncoder1Config ();
    UsartEncoder2Config ();
    UsartEncoder3Config ();
    UsartEncoder4Config ();

    UsartMainSend("\r\n-- USING ENCODER: 1 to 4 PowerUp ShuttingDown --\r\n");
    
    unsigned short state = 0;
    int cnt = 0;
    while (1)
    {
	if (!timer_standby)
	{
	    timer_standby = 1000;
	
	    switch (state)
	    {
	    case 0:
		// powerup 1 to 4
		Encoder_Write_Address_Plain (ENCOD_1, 0x7000, 0x0010);
		Encoder_Write_Address_Plain (ENCOD_2, 0x7000, 0x000B);
		Encoder_Write_Address_Plain (ENCOD_3, 0x7000, 0x0010);
		Encoder_Write_Address_Plain (ENCOD_4, 0x7000, 0x000B);
		state++;
		break;

	    case 1:
		// blank 1 and 4
		Encoder_Write_Address_Plain (ENCOD_1, 0x7000, 0x000F);
		Encoder_Write_Address_Plain (ENCOD_2, 0x7000, 0x000A);
		Encoder_Write_Address_Plain (ENCOD_3, 0x7000, 0x000F);
		Encoder_Write_Address_Plain (ENCOD_4, 0x7000, 0x000A);
		
		if (cnt < 5)
		{
		    cnt++;
		    state = 0;
		}
		else
		{
		    cnt = 0;
		    state++;
		}
		break;

	    case 2:
		// shutting down 1 and 4
		Encoder_Write_Address_Plain (ENCOD_1, 0x7000, 0x0011);
		Encoder_Write_Address_Plain (ENCOD_2, 0x7000, 0x000C);
		Encoder_Write_Address_Plain (ENCOD_3, 0x7000, 0x0011);
		Encoder_Write_Address_Plain (ENCOD_4, 0x7000, 0x000C);
		state++;
		break;

	    case 3:
		// blank 1 and 4
		Encoder_Write_Address_Plain (ENCOD_1, 0x7000, 0x000F);
		Encoder_Write_Address_Plain (ENCOD_2, 0x7000, 0x000A);
		Encoder_Write_Address_Plain (ENCOD_3, 0x7000, 0x000F);
		Encoder_Write_Address_Plain (ENCOD_4, 0x7000, 0x000A);		
		
		if (cnt < 5)
		{
		    cnt++;
		    state = 2;
		}
		else
		{
		    cnt = 0;
		    state = 0;
		}
		break;
	    }
	}
    }
}

//--- end of file ---//
