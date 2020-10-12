//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### TIM.C ################################
//---------------------------------------------
#ifndef _TIM_H_
#define _TIM_H_

// Exported Types Constants and Macros -----------------------------------------

#define DUTY_NONE    0
#define DUTY_10_PERCENT    100
#define DUTY_50_PERCENT    500
#define DUTY_95_PERCENT    950
#define DUTY_100_PERCENT    1000
#define DUTY_ALWAYS    (DUTY_100_PERCENT + 1)

// #define LOW_LEFT_PWM(X)    Update_TIM3_CH1(X)
#define HIGH_LEFT(X)    Update_TIM3_CH2(X)
#define LOW_RIGHT(X)    Update_TIM3_CH3(X)
// #define HIGH_RIGHT_PWM(X)    Update_TIM3_CH4(X)

#define ENABLE_TIM3    (TIM3->CR1 |= TIM_CR1_CEN)
#define DISABLE_TIM3    (TIM3->CR1 &= ~TIM_CR1_CEN)

//--- Exported macro ---//
#define RCC_TIM1_CLK    (RCC->APB2ENR & 0x00000800)
#define RCC_TIM1_CLK_ON    (RCC->APB2ENR |= 0x00000800)
#define RCC_TIM1_CLK_OFF    (RCC->APB2ENR &= ~0x00000800)

#define RCC_TIM3_CLK    (RCC->APB1ENR & 0x00000002)
#define RCC_TIM3_CLK_ON    (RCC->APB1ENR |= 0x00000002)
#define RCC_TIM3_CLK_OFF    (RCC->APB1ENR &= ~0x00000002)

#define RCC_TIM6_CLK    (RCC->APB1ENR & 0x00000010)
#define RCC_TIM6_CLK_ON    (RCC->APB1ENR |= 0x00000010)
#define RCC_TIM6_CLK_OFF    (RCC->APB1ENR &= ~0x00000010)

#define RCC_TIM14_CLK    (RCC->APB1ENR & 0x00000100)
#define RCC_TIM14_CLK_ON    (RCC->APB1ENR |= 0x00000100)
#define RCC_TIM14_CLK_OFF    (RCC->APB1ENR &= ~0x00000100)

#define RCC_TIM15_CLK    (RCC->APB2ENR & 0x00010000)
#define RCC_TIM15_CLK_ON    (RCC->APB2ENR |= 0x00010000)
#define RCC_TIM15_CLK_OFF    (RCC->APB2ENR &= ~0x00010000)

#define RCC_TIM16_CLK    (RCC->APB2ENR & 0x00020000)
#define RCC_TIM16_CLK_ON    (RCC->APB2ENR |= 0x00020000)
#define RCC_TIM16_CLK_OFF    (RCC->APB2ENR &= ~0x00020000)

#define RCC_TIM17_CLK    (RCC->APB2ENR & 0x00040000)
#define RCC_TIM17_CLK_ON    (RCC->APB2ENR |= 0x00040000)
#define RCC_TIM17_CLK_OFF    (RCC->APB2ENR &= ~0x00040000)

//--- Exported functions ---//
void UpdateTIMSync (unsigned short);
void TIM_1_Init(void);
void Update_TIM1_CH1 (unsigned short);
void TIM3_IRQHandler (void);
void TIM_3_Init(void);
void TIM_6_Init (void);
void TIM14_IRQHandler (void);
void TIM_14_Init(void);
void TIM16_IRQHandler (void);
void TIM_16_Init(void);
void TIM17_IRQHandler (void);
void TIM_17_Init(void);
void Update_TIM3_CH1 (unsigned short);
void Update_TIM3_CH2 (unsigned short);
void Update_TIM3_CH3 (unsigned short);
void Update_TIM3_CH4 (unsigned short);

void TIM1_ChangeTick (unsigned short);
    
void OneShootTIM16 (unsigned short);
void TIM16Enable (void);
void TIM16Disable (void);

void Wait_ms (unsigned short wait);

#endif    /* _TIM_H_ */

//--- end of file ---//
