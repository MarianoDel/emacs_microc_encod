//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### I2C.H #################################
//---------------------------------------------
#ifndef _I2C_H_
#define _I2C_H_

// Config Defines --------------------------------------------------------------
#define I2C_USE_I2C1
#define I2C_USE_I2C2
// #define I2C1_WITH_INTS
// #define I2C2_WITH_INTS


// Module Exported Functions ---------------------------------------------------
void I2C1_Init (void);
void I2C1_Shutdown (void);
void I2C1_OwnAddress (unsigned char own_addr);
void I2C1_Ack (unsigned char set_reset);
void I2C1_SendByte (unsigned char addr, unsigned char data);
unsigned char I2C1_SendAddr (unsigned char addr);
unsigned char I2C1_SendMultiByte (unsigned char *pdata, unsigned char addr, unsigned short size);
void I2C1_SendMultiByte_Int (unsigned char addr, unsigned char *pdata, unsigned short size);
unsigned char I2C1_CheckEnded_Int (void);

void I2C2_Init (void);
void I2C2_SendByte (unsigned char, unsigned char);
unsigned char I2C2_SendAddr (unsigned char addr);
unsigned char I2C2_SendMultiByte (unsigned char *pdata, unsigned char addr, unsigned short size);

#endif    /* _I2C_H_ */

//--- end of file ---//

