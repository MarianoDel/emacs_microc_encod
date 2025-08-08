//------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### NEOPIXEL.C ###############################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "neopixel.h"


// Module Private Types Constants & Macros -------------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
unsigned char pwm_ccr2[SIZEOF_PIXEL_DATA] = { 0 };


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void Neo_Set_Pixel (unsigned char pixel_index, pixel_t * ppixel_value)
{
    unsigned short buff_index = 0;
    unsigned char set_val = 0;
    unsigned char bits_value = 0;
    unsigned char mask = 0;

    buff_index = pixel_index * 3 * 8 + (pixel_index * PIXEL_SPACE);

    // set G
    buff_index += 0;
    bits_value = ppixel_value->G;
    mask = 0x80;
    for (int i = 0; i < 8; i++)
    {
        if (bits_value & mask)
            set_val = 53;
        else
            set_val = 27;
        
        pwm_ccr2[buff_index + i] = set_val;
        mask >>= 1;
    }
    

    // set R
    buff_index += 8;
    bits_value = ppixel_value->R;
    mask = 0x80;    
    for (int i = 0; i < 8; i++)
    {
        if (bits_value & mask)
            set_val = 53;
        else
            set_val = 27;
        
        pwm_ccr2[buff_index + i] = set_val;
        mask >>= 1;
    }

    // set B
    buff_index += 8;
    bits_value = ppixel_value->B;
    mask = 0x80;    
    for (int i = 0; i < 8; i++)
    {
        if (bits_value & mask)
            set_val = 53;
        else
            set_val = 27;
        
        pwm_ccr2[buff_index + i] = set_val;
        mask >>= 1;
    }    
}

//--- end of file ---//
