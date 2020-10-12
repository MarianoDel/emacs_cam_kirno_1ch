//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### GPIO.C ################################
//---------------------------------------------
#include "stm32f0xx.h"
#include "gpio.h"
#include "hard.h"



//--- Private typedef ---//
//--- Private define ---//
//--- Private macro ---//
//--- Private variables ---//
//--- Private function prototypes ---//
//--- Private functions ---//

//-------------------------------------------//
// @brief  GPIO configure.
// @param  None
// @retval None
//------------------------------------------//
void GPIO_Config (void)
{
    unsigned long temp;

    //--- MODER ---//
    //00: Input mode (reset state)
    //01: General purpose output mode
    //10: Alternate function mode
    //11: Analog mode

    //--- OTYPER ---//
    //These bits are written by software to configure the I/O output type.
    //0: Output push-pull (reset state)
    //1: Output open-drain

    //--- ORSPEEDR ---//
    //These bits are written by software to configure the I/O output speed.
    //x0: Low speed.
    //01: Medium speed.
    //11: High speed.
    //Note: Refer to the device datasheet for the frequency.

    //--- PUPDR ---//
    //These bits are written by software to configure the I/O pull-up or pull-down
    //00: No pull-up, pull-down
    //01: Pull-up
    //10: Pull-down
    //11: Reserved

    //--- GPIO A ---//
    if (!GPIOA_CLK)
        GPIOA_CLK_ON;

    temp = GPIOA->MODER;       //2 bits por pin
    temp &= 0x3CC33F00;        //PA0 out; PA1 analog; PA2 input; PA3 analog; PA7 alternate function
    temp |= 0x012880CD;        //PA9 PA10 alternative; PA12 out; PA15 input
    GPIOA->MODER = temp;

    temp = GPIOA->OTYPER;	//1 bit por pin
#ifdef USART_TX_OUTPUT_OPEN_DRAIN
    temp &= 0xFFFFFDFF;
    temp |= 0x00000200;        //open drain PA10
#else
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000; 
#endif
    GPIOA->OTYPER = temp;

    temp = GPIOA->OSPEEDR;     //2 bits por pin
    temp &= 0xFCF33FFC;        //PA0 PA7 PA9 PA12 low speed
    temp |= 0x00000000;
    GPIOA->OSPEEDR = temp;

    temp = GPIOA->PUPDR;       //2 bits por pin
#ifdef USART_RX_PULLUP    
    temp &= 0x3FCFFFFF;        //PA15 PA10 pullup
    temp |= 0x40100000;
#else
    temp &= 0x3FFFFFFF;        //PA15 pullup
    temp |= 0x40000000;
#endif
    GPIOA->PUPDR = temp;

    
    //--- GPIO B ---//
#ifdef GPIOB_ENABLE
    if (!GPIOB_CLK)
        GPIOB_CLK_ON;

    temp = GPIOB->MODER;       //2 bits por pin
    temp &= 0xFFFFCC3C;        //PB0 alternative; PB3 PB4 PB6 input
    temp |= 0x00000002;
    GPIOB->MODER = temp;

    temp = GPIOB->OTYPER;	//1 bit por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOB->OTYPER = temp;

    temp = GPIOB->OSPEEDR;     //2 bits por pin
    temp &= 0xFFFFFFFC;        //PB0 low speed
    temp |= 0x00000000;
    GPIOB->OSPEEDR = temp;

    temp = GPIOB->PUPDR;       //2 bits por pin
    temp &= 0xFFFFCC3F;        //PB3 PB4 PB6 pull up
    temp |= 0x00001140;
    GPIOB->PUPDR = temp;

#endif

#ifdef GPIOF_ENABLE

    //--- GPIO F ---//
    if (!GPIOF_CLK)
        GPIOF_CLK_ON;

    temp = GPIOF->MODER;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->MODER = temp;

    temp = GPIOF->OTYPER;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->OTYPER = temp;

    temp = GPIOF->OSPEEDR;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->OSPEEDR = temp;

    temp = GPIOF->PUPDR;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->PUPDR = temp;

#endif


#ifdef USE_PROTECTION_WITH_INT
    //Interrupt en PA2
    if (!SYSCFG_CLK)
        SYSCFG_CLK_ON;

    SYSCFG->EXTICR[0] = 0x00000000; //Select Port A & Pin 2 external interrupt
    EXTI->IMR |= 0x00000004; 			//Corresponding mask bit for interrupts EXTI0
    EXTI->EMR |= 0x00000000; 			//Corresponding mask bit for events
    EXTI->RTSR |= 0x00000004; 			//Pin 2 Interrupt line on rising edge
    EXTI->FTSR |= 0x00000000; 			//Pin 2 Interrupt line on falling edge

    NVIC_EnableIRQ(EXTI2_3_IRQn);
    NVIC_SetPriority(EXTI2_3_IRQn, 3);
#endif
}

inline void EXTIOff (void)
{
	EXTI->IMR &= ~0x00000004;
}

inline void EXTIOn (void)
{
	EXTI->IMR |= 0x00000004;
}

//--- end of file ---//
