//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### CRC_ENC.H ###############################
//---------------------------------------------
#ifndef _CRC_ENC_H_
#define _CRC_ENC_H_


// Module Exported functions ---------------------------------------------------
unsigned short CRC16(unsigned char *puchMsg,unsigned short usDataLen);
unsigned short CRC16_Swap (unsigned short crc_to_swap);


#endif    /* _CRC_ENC_H_ */

//--- end of file ---//

