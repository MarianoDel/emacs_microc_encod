//------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### NEOPIXEL.H ###############################
//------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _NEOPIXEL_H_
#define _NEOPIXEL_H_


// Module Exported Types Constants and Macros ----------------------------------
#define PIXEL_QTTY    64    // how many leds
#define PIXEL_SPACE    4

#define SIZEOF_PIXEL_DATA    ((PIXEL_QTTY * 3 * 8) + (PIXEL_SPACE * (PIXEL_QTTY - 1)) + 1)

typedef struct {
    unsigned char R;
    unsigned char G;
    unsigned char B;
    
} pixel_t;


// Module Exported Functions ---------------------------------------------------
void Neo_Set_Pixel (unsigned char pixel_index, pixel_t * ppixel_value);


#endif    /* _NEOPIXEL_H_ */

//--- end of file ---//

