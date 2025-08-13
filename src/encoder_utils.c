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
#define CMD_TOUCH    3
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
    else if (buff_rx [3] == 0x41)
    {
	cmd = CMD_TOUCH;
	UsartMainSend(" cmd: touch\r\n");
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
	    UsartMainSend(" crc: ok\r\n");
	}
	else
	{
	    UsartMainSend(" crc: nok!\r\n");
	}

	if ((bytes_read - 3) == buff_rx[2])
	{
	    sprintf(buff_to_send," len: ok read: %d buffr: %d\r\n", bytes_read, buff_rx[2]);
	    UsartMainSend(buff_to_send);
	}
	else
	{
	    sprintf(buff_to_send," len: nok! read: %d buffr: %d\r\n", bytes_read, buff_rx[2]);
	    UsartMainSend(buff_to_send);
	}

	return;
    }
    else if (cmd == CMD_READ)
    {
	int r = 0;
	if ((buff_rx [4] == 0xff) &&
	    (buff_rx [5] == 0x41) &&
	    (buff_rx [6] == 0x00))
	{
	    UsartMainSend(" crc: ok\r\n");
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
    else
    {
	// CMD_TOUCH
	int r = 0;
	if ((bytes_read - 3) == buff_rx[2])
	{
	    UsartMainSend(" len: ok\r\n");
	}
	else if (buff_rx[2] < 7)
	{
	    sprintf(buff_to_send," len: less than 7 read: %d buffr: %d\r\n", bytes_read, buff_rx[2]);
	    UsartMainSend(buff_to_send);
	    r = 1;
	}
	else
	{
	    sprintf(buff_to_send," len: nok! read: %d buffr: %d\r\n", bytes_read, buff_rx[2]);
	    UsartMainSend(buff_to_send);
	    r = 1;
	}

	if (r)
	    return;

	int base = 0;
	if (buff_rx[2] == 0x23)    // timer or encoder len
	{
	    for (int i = 0; i < 8; i++)
	    {
		base = i * 4 + 4;
		sprintf(buff_to_send, "  reg%d: %02x%02x value: %02x%02x\r\n",
			i,
			buff_rx[base + 0],
			buff_rx[base + 1],
			buff_rx[base + 2],
			buff_rx[base + 3]);
		UsartMainSend(buff_to_send);
	    }	    
	}
	else
	{
	    sprintf(buff_to_send," addr/reg: %02x%02x\r\n", buff_rx[4], buff_rx[5]);
	    UsartMainSend(buff_to_send);
	    sprintf(buff_to_send," bytes get: %02x B: %d\r\n", buff_rx[2], buff_rx[2] - 5);
	    UsartMainSend(buff_to_send);
	    
	    for (int i = 0; i < buff_rx[2] - 5; i++)
	    {
		base = i * 2 + 6;
		sprintf(buff_to_send, "  value%d: %02x%02x\r\n", i + 1, buff_rx[base], buff_rx[base + 1]);
		UsartMainSend(buff_to_send);
	    }
	}
    }
}


unsigned char Encoder_Process_Buffer_Plain (int bytes_read, unsigned char * buff_rx)
{
    unsigned char cmd = 0;
    
    // disarm packet
    if ((buff_rx [0] != 0x5a) &&
	(buff_rx [1] != 0xa5))
    {
	return BUF_HEADER_NOK;
    }

    if (buff_rx [3] == 0x10)
    {
	cmd = CMD_WRITE;
    }
    else if (buff_rx [3] == 0x03)
    {
	cmd = CMD_READ;
    }
    else if (buff_rx [3] == 0x41)
    {
	cmd = CMD_TOUCH;
    }
    else
    {
	return BUF_UNKNOW;
    }
	
    if (cmd == CMD_WRITE)
    {
	if ((buff_rx [4] != 0xff) ||
	    (buff_rx [5] != 0x4c) ||
	    (buff_rx [6] != 0x30))
	{
	    return BUF_WRITE_CRC_NOK;	    
	}

	if ((bytes_read - 3) != buff_rx[2])
	    return BUF_WRITE_CRC_OK_LEN_NOK;
	    
	return BUF_WRITE_CRC_OK_LEN_OK;
    }
    else if (cmd == CMD_READ)
    {
	if ((buff_rx [4] != 0xff) ||
	    (buff_rx [5] != 0x41) ||
	    (buff_rx [6] != 0x00))
	{
	    return BUF_READ_CRC_NOK;
	}

	if ((bytes_read - 3) == buff_rx[2])
	{
	    return BUF_READ_CRC_OK_LEN_NO;    // no further data 
	}
	else if ((bytes_read - 3) < buff_rx[2])
	{
	    return BUF_READ_CRC_OK_LEN_NOK;    // len nok 
	}

	// sprintf(buff_to_send," addr read: %02x%02x\r\n", buff_rx[11], buff_rx[12]);
	// UsartMainSend(buff_to_send);
	// sprintf(buff_to_send," bytes read: %02x%02x B: %d\r\n", buff_rx[13], buff_rx[14], buff_rx[14]);
	// UsartMainSend(buff_to_send);

	// int base = 0;
	// for (int i = 0; i < buff_rx[14]; i++)
	// {
	//     base = i * 2 + 15;
	//     sprintf(buff_to_send, "  value%d: %02x%02x\r\n", i + 1, buff_rx[base], buff_rx[base + 1]);
	//     UsartMainSend(buff_to_send);
	// }
	return BUF_READ_CRC_OK_LEN_OK;
	
    }
    else
    {
	// CMD_TOUCH
	if ((bytes_read - 3) != buff_rx[2])
	{
	    return BUF_TOUCH_LEN_NOK;
	}


	if (buff_rx[2] != 0x23)    // timer or encoder len
	{
	    return BUF_TOUCH_NO_TIMER_NO_ENCOD;
	}

	// int base = 0;
	// for (int i = 0; i < 8; i++)
	// {
	//     base = i * 4 + 4;
	//     sprintf(buff_to_send, "  reg%d: %02x%02x value: %02x%02x\r\n",
	// 	    i,
	// 	    buff_rx[base + 0],
	// 	    buff_rx[base + 1],
	// 	    buff_rx[base + 2],
	// 	    buff_rx[base + 3]);
	//     UsartMainSend(buff_to_send);
	// }
	return BUF_TOUCH_TIMER_OR_ENCOD;	
    }
    
    return BUF_UNKNOW;
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


void Encoder_Write_Address_Plain (unsigned char enc_number,
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


void Encoder_Read_Address_Plain (unsigned char enc_number,
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
}

//--- end of file ---//
