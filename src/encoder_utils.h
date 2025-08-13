//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### ENCODER_UTILS.H #######################
//---------------------------------------------
#ifndef _ENCODER_UTILS_H_
#define _ENCODER_UTILS_H_


// Module Exported Types Constants and Macros ----------------------------------
#define ENCOD_1    0
#define ENCOD_2    1
#define ENCOD_3    2
#define ENCOD_4    3

#define BUF_HEADER_NOK    0
#define BUF_UNKNOW    1
#define BUF_WRITE_CRC_NOK    2
#define BUF_WRITE_CRC_OK_LEN_NOK    3
#define BUF_WRITE_CRC_OK_LEN_OK    4
#define BUF_READ_CRC_NOK    5
#define BUF_READ_CRC_OK_LEN_NO    6
#define BUF_READ_CRC_OK_LEN_NOK    7
#define BUF_READ_CRC_OK_LEN_OK    8
#define BUF_TOUCH_LEN_NOK    9
#define BUF_TOUCH_NO_TIMER_NO_ENCOD    10
#define BUF_TOUCH_TIMER_OR_ENCOD    11



// Module Exported functions ---------------------------------------------------
void Encoder_Process_Buffer (int bytes_read, unsigned char * buff_rx);

unsigned char Encoder_Process_Buffer_Plain (int bytes_read, unsigned char * buff_rx);

void Encoder_Write_Address (unsigned char enc_number,
			    unsigned short addr,
			    unsigned short value);

void Encoder_Write_Address_Plain (unsigned char enc_number,
				  unsigned short addr,
				  unsigned short value);

void Encoder_Read_Address (unsigned char enc_number,
			   unsigned short addr,
			   unsigned short bytes_to_read);

void Encoder_Read_Address_Plain (unsigned char enc_number,
				 unsigned short addr,
				 unsigned short bytes_to_read);

#endif    /* _ENCODER_UTILS_H_ */

//--- end of file ---//

