//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### USART.H ###############################
//---------------------------------------------
#ifndef _USART_H_
#define _USART_H_

// Exported Types Constants and Macros -----------------------------------------
#define USART1_CLK    (RCC->APB2ENR & 0x00004000)
#define USART1_CLK_ON    (RCC->APB2ENR |= 0x00004000)
#define USART1_CLK_OFF    (RCC->APB2ENR &= ~0x00004000)

#define USART_9600    5000
#define USART_115200    416
#define USART_250000    192

#define SIZEOF_RXDATA    128
#define SIZEOF_TXDATA    128


// Module Exported Functions ---------------------------------------------------
void Usart1Config (void);
void Usart1SendSingle (unsigned char);
void Usart1Send (char *);
void Usart1SendUnsigned (unsigned char *, unsigned char);
unsigned char Usart1ReadBuffer (unsigned char *, unsigned short);
unsigned char Usart1HaveData (void);
void Usart1HaveDataReset (void);
void USART1_IRQHandler(void);

#endif    /* _USART_H_ */

//--- end of file ---//

