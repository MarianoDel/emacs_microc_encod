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


// Module Exported functions ---------------------------------------------------
void Encoder_Process_Buffer(int bytes_read, unsigned char * buff_rx);

void Encoder_Write_Address (unsigned char enc_number,
			    unsigned short addr,
			    unsigned short value);

void Encoder_Read_Address (unsigned char enc_number,
			   unsigned short addr,
			   unsigned short bytes_to_read);


#endif    /* _ENCODER_UTILS_H_ */

//--- end of file ---//

