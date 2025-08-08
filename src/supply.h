//------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### SUPPLY.H #################################
//------------------------------------------------
#ifndef _SUPPLY_H_
#define _SUPPLY_H_



// Module Exported Types Constants and Macros ----------------------------------
// inputs divider/multiplier 0.211 or 261.8 per volt
// #define MAINS_11V    2887    // 11V
#define MAINS_11V    2771    // 11V ajusto 4%
// #define MAINS_10V    2625    // 10V
#define MAINS_10V    2520    // 10V ajusto 4%

#define BATT_7_2V    1885    // 7.2V
#define BATT_7V    1832    // 7V
#define BATT_6_8V    1780    // 6.8V
#define BATT_6_4V    1675    // 6.4V
// #define BATT_ADC_2    1911    // 7.3V
// #define BATT_ADC_3    2042    // 7.8V
// #define BATT_ADC_4    2147    // 8.2V

#define BOOST_13V    3403    // 13V
// #define BOOST_13_8V    3612    // 13.8V midio 14.4V
#define BOOST_13_8V    3461    // 13.8V bajo 4%
#define BOOST_14_5V    3796    // 14.5V
#define BOOST_15V    3927    // 15V

// for tests only
// #define SUPPLY_DISCONNECT_VOLTAGE    1964    // 7.5V
// #define SUPPLY_CONNECT_VOLTAGE    2094    // 8V
#define SUPPLY_MODE_UNKNOW    0
#define SUPPLY_MODE_BATT    1
#define SUPPLY_MODE_MAINS    2


// Exported Module Functions ---------------------------------------------------
void Supply_Status_Reset (void);
void Supply_Status (void);
void Supply_Timeouts (void);

void Supply_Send_Voltage_Start (void);
void Supply_Convert_To_Volts (unsigned short adc_value,
			      unsigned char * v_int,
			      unsigned char * v_dec);

unsigned short Supply_Get_Mains (void);
unsigned short Supply_Get_Boost (void);
unsigned short Supply_Get_Batt (unsigned char batt_num);

unsigned char Supply_Get_Mode (void);


#endif    /* _SUPPLY_H_ */

//--- end of file ---//

