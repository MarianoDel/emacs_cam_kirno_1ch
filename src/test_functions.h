//------------------------------------------------
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.H #########################
//------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _TEST_FUNTIONS_H_
#define _TEST_FUNTIONS_H_



// Exported Types Constants and Macros -----------------------------------------


// Exported Functions ----------------------------------------------------------
void TF_Led (void);
void TF_Led_Blinking (void);
void TF_Buzzer (void);
void TF_Gpio_Input (void);

void TF_Usart1_Single (void);
void TF_Usart1_Multiple (void);
void TF_Usart1_TxRx (void);
void TF_Usart1_Adc (void);
void TF_Usart1_Adc_Dma (void);

void TF_Tim3_Pwm (void);

#endif    /* _TEST_FUNTIONS_H_ */

//--- end of file ---//

