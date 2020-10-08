//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### UART.C ################################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "hard.h"
#include "stm32f0xx.h"
#include "uart.h"

#include <string.h>




//--- Private typedef ---//
//--- Private define ---//
//--- Private macro ---//

//#define USE_USARTx_TIMEOUT



//--- Externals variables ---//
extern volatile unsigned char usart1_have_data;

//de signals para sync
extern volatile unsigned char sync_on_signal;

//--- Private variables ---//
volatile unsigned char * ptx1;
volatile unsigned char * ptx1_pckt_index;
volatile unsigned char * prx1;
volatile unsigned char tx1buff[SIZEOF_TXDATA];
volatile unsigned char rx1buff[SIZEOF_RXDATA];


//Reception buffer.

//Transmission buffer.

//--- Private function prototypes ---//
//--- Private functions ---//


unsigned char ReadUsart1Buffer (unsigned char * bout, unsigned short max_len)
{
	unsigned int len;

	len = prx1 - rx1buff;

	if (len < max_len)
	{
		//el prx1 siempre llega adelantado desde la int, lo corto con un 0
		*prx1 = '\0';
		prx1++;
		len += 1;
		memcpy(bout, (unsigned char *) rx1buff, len);
	}
	else
	{
		memcpy(bout, (unsigned char *) rx1buff, len);
		len = max_len;
	}

	//ajusto punteros de rx luego de la copia
	prx1 = rx1buff;

	return (unsigned char) len;
}

void USART1_IRQHandler(void)
{
    unsigned char dummy;

    /* USART in Receive mode --------------------------------------------------*/
    if (USART1->ISR & USART_ISR_RXNE)
    {
        dummy = USART1->RDR & 0x0FF;

        //para sincronismo
        if (dummy == SYNC_CHAR)
        {
            sync_on_signal = 1;
#ifdef LED_SHOW_SYNC_UART
            if (LED)
                LED_OFF;
            else
                LED_ON;
#endif
        }
        else
        {
            if (prx1 < &rx1buff[SIZEOF_RXDATA - 1])
            {
                if ((dummy == '\n') || (dummy == '\r') || (dummy == 26))		//26 es CTRL-Z
                {
                    *prx1 = '\0';
                    usart1_have_data = 1;
                }
                else
                {
                    *prx1 = dummy;
                    prx1++;
                }
            }
            else
                prx1 = rx1buff;    //soluciona problema bloqueo con garbage
        }
    }

    /* USART in Transmit mode -------------------------------------------------*/

    if (USART1->CR1 & USART_CR1_TXEIE)
    {
        if (USART1->ISR & USART_ISR_TXE)
        {
            if ((ptx1 < &tx1buff[SIZEOF_TXDATA]) && (ptx1 < ptx1_pckt_index))
            {
                USART1->TDR = *ptx1;
                ptx1++;
            }
            else
            {
                ptx1 = tx1buff;
                ptx1_pckt_index = tx1buff;
                USART1->CR1 &= ~USART_CR1_TXEIE;
            }
        }
    }

    if ((USART1->ISR & USART_ISR_ORE) || (USART1->ISR & USART_ISR_NE) || (USART1->ISR & USART_ISR_FE))
    {
        USART1->ICR |= 0x0e;
        dummy = USART1->RDR;
    }
}

void Usart1Send (char * send)
{
	unsigned char i;

	i = strlen(send);
	Usart1SendUnsigned((unsigned char *) send, i);
}

void Usart1SendUnsigned(unsigned char * send, unsigned char size)
{
	if ((ptx1_pckt_index + size) < &tx1buff[SIZEOF_TXDATA])
	{
		memcpy((unsigned char *)ptx1_pckt_index, send, size);
		ptx1_pckt_index += size;
		USART1->CR1 |= USART_CR1_TXEIE;
	}
}

void Usart1SendSingle(unsigned char tosend)
{
	Usart1SendUnsigned(&tosend, 1);
}


void USART1Config(void)
{
	unsigned int temp;

	if (!USART1_CLK)
		USART1_CLK_ON;


	ptx1 = tx1buff;
	ptx1_pckt_index = tx1buff;
	prx1 = rx1buff;

	USART1->BRR = USART_9600;
//	USART1->CR2 |= USART_CR2_STOP_1;	//2 bits stop
//	USART1->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
//	USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_UE;	//SIN TX
	USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;	//para pruebas TX

	temp = GPIOA->AFR[1];
	temp &= 0xFFFFF00F;
	temp |= 0x00000110;	//PA10 -> AF1 PA9 -> AF1
	GPIOA->AFR[1] = temp;

	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn, 7);
}


//--- end of file ---//
