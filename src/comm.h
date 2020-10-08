//------------------------------------------------
// #### PROYECTO STRETCHER F030 - Power Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### COMM.H ###################################
//------------------------------------------------
#ifndef _COMM_H_
#define _COMM_H_


//--- Exported types ---//

//--- Exported constants ---//
typedef enum {
	resp_ok = 0,
	resp_not_own,
	resp_error,
        resp_ended,
        resp_continue

} resp_t;

//--- Exported macro ---//

//--- Exported functions ---//
void UpdateCommunications (void);
unsigned char SerialProcess (void);
unsigned char InterpretarMsg (void);
void SetOwnChannel (unsigned char);
char * GetOwnChannel (void);


#endif    /* _COMM_H_ */

//--- end of file ---//
