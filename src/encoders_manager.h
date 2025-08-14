//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### ENCODERS_MANAGER.H #######################
//---------------------------------------------
#ifndef _ENCODERS_MANAGER_H_
#define _ENCODERS_MANAGER_H_


// Module Exported Types Constants and Macros ----------------------------------



// Module Exported functions ---------------------------------------------------
void Encoders_Manager_Timeouts (void);
void Encoders_Manager_Update (void);
unsigned char Encoders_Manager_Rpi_Get (void);
void Encoders_Manager_Rpi_Set (unsigned char new_rpi_state);
void Encoders_Manager_Buffer_Update (unsigned char encoder,
				     unsigned char new_position);



#endif    /* _ENCODERS_MANAGER_H_ */

//--- end of file ---//

