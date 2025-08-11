//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### ENCODER_UTILS.C #######################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "encoder_utils.h"
#include "usart_channels.h"
#include "crc_enc.h"

#include <stdio.h>


// Module Functions ------------------------------------------------------------
#define CMD_READ    1
#define CMD_WRITE    2
void Encoder_Process_Buffer(int bytes_read, unsigned char * buff_rx)
{
    char buff_to_send [256];
    unsigned char cmd = 0;
    
    // disarm packet
    if ((buff_rx [0] != 0x5a) &&
	(buff_rx [1] != 0xa5))
    {
	sprintf(buff_to_send,"header: nok! length: %d ", buff_rx [2]);
	UsartMainSend(buff_to_send);
	return;
    }

    sprintf(buff_to_send,"header: ok length: %d\r\n", buff_rx [2]);
    UsartMainSend(buff_to_send);

    if (buff_rx [3] == 0x10)
    {
	cmd = CMD_WRITE;
	UsartMainSend(" cmd: write\r\n");
    }
    else if (buff_rx [3] == 0x03)
    {
	cmd = CMD_READ;
	UsartMainSend(" cmd: read\r\n");
    }
    else
    {
	UsartMainSend(" cmd: unknow\r\n");
	return;
    }
	
    if (cmd == CMD_WRITE)
    {
	if ((buff_rx [4] == 0xff) &&
	    (buff_rx [5] == 0x4c) &&
	    (buff_rx [6] == 0x30))
	{
	    UsartMainSend("crc ok ");
	}
	else
	{
	    UsartMainSend("crc: nok! ");
	}

	if ((bytes_read - 3) == buff_rx[2])
	{
	    sprintf(buff_to_send,"len: ok read: %d buffr: %d\n", bytes_read, buff_rx[2]);
	    UsartMainSend(buff_to_send);
	}
	else
	{
	    sprintf(buff_to_send,"len: nok! read: %d buffr: %d\n", bytes_read, buff_rx[2]);
	    UsartMainSend(buff_to_send);
	}

	return;
    }
    else
    {
	int r = 0;
	// cmd == CMD_READ
	if ((buff_rx [4] == 0xff) &&
	    (buff_rx [5] == 0x41) &&
	    (buff_rx [6] == 0x00))
	{
	    UsartMainSend(" crc ok\r\n");
	}
	else
	{
	    UsartMainSend(" crc: nok!\r\n");
	    r = 1;
	}

	if ((bytes_read - 3) == buff_rx[2])
	{
	    UsartMainSend(" len: ok no further data!\r\n");
	    r = 1;
	}
	else if ((bytes_read - 3) > buff_rx[2])
	{
	    sprintf(buff_to_send," len: further data read: %d buffr: %d\r\n", bytes_read, buff_rx[2]);
	    UsartMainSend(buff_to_send);
	}
	else
	{
	    sprintf(buff_to_send," len: nok! read: %d buffr: %d\r\n", bytes_read, buff_rx[2]);
	    UsartMainSend(buff_to_send);
	    r = 1;
	}

	if (r)
	    return;
	
    }

    if (bytes_read < 15)
    {
	sprintf(buff_to_send," length: nok! len: %d\r\n", bytes_read);
	UsartMainSend(buff_to_send);
	return;
    }
	
    if ((buff_rx [7] != 0x5a) &&
	(buff_rx [8] != 0xa5))
    {
	sprintf(buff_to_send," header: nok! length: %d\r\n", buff_rx [9]);
	UsartMainSend(buff_to_send);
	return;
    }
    
    if (buff_rx [10] != 0x03)
    {
	sprintf(buff_to_send," cmd: nok! length: %d\r\n", buff_rx [9]);
	UsartMainSend(buff_to_send);
	return;	
    }

    sprintf(buff_to_send," addr read: %02x%02x\r\n", buff_rx[11], buff_rx[12]);
    UsartMainSend(buff_to_send);
    sprintf(buff_to_send," bytes read: %02x%02x B: %d\r\n", buff_rx[13], buff_rx[14], buff_rx[14]);
    UsartMainSend(buff_to_send);

    int base = 0;
    for (int i = 0; i < buff_rx[14]; i++)
    {
	base = i * 2 + 15;
	sprintf(buff_to_send, "  value%d: %02x%02x\r\n", i + 1, buff_rx[base], buff_rx[base + 1]);
	UsartMainSend(buff_to_send);
    }
}


void Encoder_Write_Address (unsigned char enc_number,
			    unsigned short addr,
			    unsigned short value)
{
    unsigned char a;
    unsigned short crc;
    unsigned short crc_swap;	    
    unsigned char buff_snd [100];

    buff_snd[0] = 0x5a;
    buff_snd[1] = 0xa5;
    buff_snd[2] = 7;
    buff_snd[3] = 0x10;
    a = (addr >> 8);
    buff_snd[4] = a;
    a = addr & 0x00ff;
    buff_snd[5] = a;
    a = (value >> 8);
    buff_snd[6] = a;
    a = value & 0x00ff;
    buff_snd[7] = a;

    crc = CRC16(buff_snd + 3, 5);
    crc_swap = CRC16_Swap (crc);

    a = (crc_swap >> 8);
    buff_snd[8] = a;
    a = crc_swap & 0x00ff;
    buff_snd[9] = a;

    switch (enc_number)
    {
    case ENCOD_1:
	UsartEncoder1SendUnsigned(buff_snd, 10);
	break;
	
    case ENCOD_2:
	UsartEncoder2SendUnsigned(buff_snd, 10);
	break;
	
    case ENCOD_3:
	UsartEncoder3SendUnsigned(buff_snd, 10);
	break;
	
    case ENCOD_4:
	UsartEncoder4SendUnsigned(buff_snd, 10);
	break;
	
    }

    char buff_to_send [120];    
    sprintf(buff_to_send, "wrt enc: %d wrote: %d crc calc: %04x crc_swap: %04x\n",
	    enc_number,
	    10,
	    crc,
	    crc_swap);
    
    UsartMainSend(buff_to_send);
    
}


void Encoder_Read_Address (unsigned char enc_number,
			   unsigned short addr,
			   unsigned short bytes_to_read)
{
    
    unsigned char a;
    unsigned short crc;
    unsigned short crc_swap;	    
    unsigned char buff_snd [100];

    buff_snd[0] = 0x5a;
    buff_snd[1] = 0xa5;
    buff_snd[2] = 7;
    buff_snd[3] = 0x03;
    a = (addr >> 8);
    buff_snd[4] = a;
    a = addr & 0x00ff;
    buff_snd[5] = a;
    a = (bytes_to_read >> 8);
    buff_snd[6] = a;
    a = bytes_to_read & 0x00ff;
    buff_snd[7] = a;

    crc = CRC16(buff_snd + 3, 5);
    crc_swap = CRC16_Swap (crc);

    a = (crc_swap >> 8);
    buff_snd[8] = a;
    a = crc_swap & 0x00ff;
    buff_snd[9] = a;

    switch (enc_number)
    {
    case ENCOD_1:
	UsartEncoder1SendUnsigned(buff_snd, 10);
	break;
	
    case ENCOD_2:
	UsartEncoder2SendUnsigned(buff_snd, 10);
	break;
	
    case ENCOD_3:
	UsartEncoder3SendUnsigned(buff_snd, 10);
	break;
	
    case ENCOD_4:
	UsartEncoder4SendUnsigned(buff_snd, 10);
	break;
	
    }

    char buff_to_send [120];    
    sprintf(buff_to_send, "rd enc: %d wrote: %d crc calc: %04x crc_swap: %04x\n",
	    enc_number,
	    10,
	    crc,
	    crc_swap);
    
    UsartMainSend(buff_to_send);
    
}

//--- end of file ---//
