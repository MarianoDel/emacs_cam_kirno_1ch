//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### TIM.C ################################
//---------------------------------------------
#include "tim.h"
#include "stm32f0xx.h"
#include "uart.h"
#include "hard.h"


//--- VARIABLES EXTERNAS ---//
extern volatile unsigned char timer_1seg;
extern volatile unsigned short timer_led_comm;
extern volatile unsigned short wait_ms_var;


//--- VARIABLES GLOBALES ---//

volatile unsigned short timer_1000 = 0;



//--- FUNCIONES DEL MODULO ---//
void UpdateTIMSync (unsigned short a)
{
    //primero cargo TIM1
    TIM1->CCR1 = a;
    TIM3->ARR = DUTY_50_PERCENT + a;    //TIM3->CCR1 es el delay entre timers
}

void Update_TIM3_CH1 (unsigned short a)
{
	TIM3->CCR1 = a;
}

void Update_TIM3_CH2 (unsigned short a)
{
	TIM3->CCR2 = a;
}

void Update_TIM3_CH3 (unsigned short a)
{
	TIM3->CCR3 = a;
}

void Update_TIM3_CH4 (unsigned short a)
{
	TIM3->CCR4 = a;
}

void Update_TIM14_CH1 (unsigned short a)
{
	TIM14->CCR1 = a;
}

void Wait_ms (unsigned short wait)
{
	wait_ms_var = wait;
	while (wait_ms_var);
}

void TIM1_ChangeTick (unsigned short new_tick)
{
    TIM1->ARR = new_tick;
}

//-------------------------------------------//
// @brief  TIM configure.
// @param  None
// @retval None
//------------------------------------------//
void TIM_1_Init (void)
{
	unsigned int temp = 0;

	if (!RCC_TIM1_CLK)
	  RCC_TIM1_CLK_ON;

	//Configuracion del timer.
	//TIM1->CR1 |= TIM_CR1_OPM;        //clk int / 1; upcounting; one pulse
	TIM1->CR1 = 0x00;        //clk int / 1;
	TIM1->CR2 |= TIM_CR2_MMS_1;        //UEV -> TRG0
	//TIM1->CR2 = 0x00;
	//TIM1->SMCR |= TIM_SMCR_MSM | TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 | TIM_SMCR_TS_1;    //link timer3
	TIM1->SMCR = 0x0000;
	//TIM1->CCMR1 = 0x6000;            //CH2 output PWM mode 1
	// TIM1->CCMR1 = 0x6060;      //CH1, CH2 output PWM mode 1 (channel active TIM1->CNT < TIM1->CCR1)
	// TIM1->CCMR2 = 0x6060;      //CH3, CH4 output PWM mode 1 (channel active TIM1->CNT < TIM1->CCR1)
	//  TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1P;
	// TIM1->CCER |= TIM_CCER_CC1E;
	// TIM1->BDTR |= TIM_BDTR_MOE;
	// TIM1->ARR = 1023;                //cada tick 20.83ns
	TIM1->ARR = 667;       	//freq 1500Hz con tick 1us


	TIM1->CNT = 0;
	TIM1->PSC = 47;	//+1 cada tick 1us

	//Configuracion Pines
	//Alternate Fuction
	//  temp = GPIOA->MODER;    //2 bits por pin
	//  temp &= 0xFFFCFFFF;        //PA8 (alternative)
	//  temp |= 0x00020000;
	//  GPIOA->MODER = temp;

	// temp = GPIOA->AFR[1];
	// temp &= 0xFFFFFFF0;
	// temp |= 0x00000002;    //PA8 -> AF2
	// GPIOA->AFR[1] = temp;

	// Enable timer ver UDIS
	//TIM1->DIER |= TIM_DIER_UIE;
	TIM1->CR1 |= TIM_CR1_CEN;

	TIM1->CCR1 = 0;
}


void TIM_3_Init (void)			//quiero algo alrededor de los 7KHz
{
    unsigned int temp = 0;

    if (!RCC_TIM3_CLK)
        RCC_TIM3_CLK_ON;

    //Configuracion del timer.
    // TIM3->CR1 |= TIM_CR1_OPM;        //clk int / 1; upcounting; one pulse
    TIM3->CR1 = 0x0000;        			//clk int / 1 no filter; upcounting;
    TIM3->CR2 |= TIM_CR2_MMS_1;        //UEV -> TRG0
    // TIM3->CR2 = 0x0000;
    //TIM3->SMCR |= TIM_SMCR_SMS_2 |TIM_SMCR_SMS_1 | TIM_SMCR_TS_1 | TIM_SMCR_TS_0;    //reset mode
    //TIM3->SMCR |= TIM_SMCR_SMS_2;    //reset mode link timer1    OJO no anda
    // TIM3->SMCR |= TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1;    //trigger mode link timer1
    //TIM3->SMCR = 0x0000;    //
    TIM3->CCMR1 = 0x6060;      //CH1, CH2 output PWM mode 1 (channel active TIM3->CNT < TIM3->CCR1)
    TIM3->CCMR2 = 0x6060;      //CH3, CH4 output PWM mode 1 (channel active TIM3->CNT < TIM3->CCR1)

    // TIM3->CCMR1 = 0x0070;            //CH1 output PWM mode 2 (channel inactive TIM3->CNT < TIM3->CCR1)
    // TIM3->CCMR2 = 0x0000;            //
    //  TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1P;    //CH1 enable on pin active high
    TIM3->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;    //CH1-4 enable on pin active high
    //TIM3->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2P;    //CH2 enable on pin active high
    TIM3->ARR = DUTY_100_PERCENT;
    TIM3->CNT = 0;
    TIM3->PSC = 6;		//+1
    //TIM3->EGR = TIM_EGR_UG;    //generate event

    //Configuracion Pines
    //Alternate Fuction
    //  temp = GPIOA->MODER;    //2 bits por pin
    //  temp &= 0xFFFFCFFF;        //PA6 (alternative)
    //  temp |= 0x00002000;
    //  GPIOA->MODER = temp;

    //Configuracion Pines LOW_LEFT HIGH_LEFT
    temp = GPIOA->AFR[0];
    temp &= 0x00FFFFFF;
    temp |= 0x11000000;    //PA6 -> AF1, PA7 -> AF1;
    GPIOA->AFR[0] = temp;

    //Configuracion Pines LOW_RIGHT HIGH_RIGHT
    temp = GPIOB->AFR[0];
    temp &= 0xFFFFFF00;
    temp |= 0x00000011;    //PB0 -> AF1, PB1 -> AF1;
    GPIOB->AFR[0] = temp;

    // Enable timer ver UDIS
    //TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CR1 |= TIM_CR1_CEN;

    // //TIM3->CCR2 = 512;        //delay = TIM3->CCRx = 512 - TIM1->CCR2
    // TIM3->CCR1 = 3429;        //delay = TIM3->CCRx = 512 - TIM1->CCR2

    // //activo una int para generar flag del PID
    // TIM3->DIER |= TIM_DIER_UIE;
    // NVIC_EnableIRQ(TIM3_IRQn);
    // NVIC_SetPriority(TIM3_IRQn, 8);
}




void TIM3_IRQHandler (void)	//alrededor de 7KHz
{
    //bajar flag
    if (TIM3->SR & 0x01)	//bajo el flag
        TIM3->SR = 0x00;
}

void TIM_6_Init (void)    //cada tick 100us
{
    if (!RCC_TIM6_CLK)
        RCC_TIM6_CLK_ON;

    //Configuracion del timer.
    TIM6->CR1 = 0x00;		//clk int / 1; upcounting
    TIM6->PSC = 4700;			
    TIM6->ARR = 0xFFFF;			//para que arranque
    TIM6->CR1 |= TIM_CR1_CEN;
}

void TIM14_IRQHandler (void)	//100uS
{

	if (TIM14->SR & 0x01)
		//bajar flag
		TIM14->SR = 0x00;
}


void TIM_14_Init (void)
{
	unsigned int temp;

	if (!RCC_TIM14_CLK)
		RCC_TIM14_CLK_ON;

	TIM14->CCMR1 = 0x0060;            //CH1 output PWM mode 1
	TIM14->CCER |= TIM_CCER_CC1E;    //CH1 enable on pin active high
	//TIM3->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2P;    //CH2 enable on pin active high
	TIM14->PSC = 3;			//tick cada 83.33n
	TIM14->ARR = 1023;    //freq 11.73KHz

	//Configuracion del timer.
	TIM14->EGR |= 0x0001;

	//Configuracion Pin PB1
	temp = GPIOB->AFR[0];
	temp &= 0xFFFFFF0F;
	temp |= 0x00000000;	//PB1 -> AF0
	GPIOB->AFR[0] = temp;

	TIM14->CR1 |= TIM_CR1_CEN;

}

void TIM16_IRQHandler (void)	//es one shoot
{
    if (TIM16->SR & 0x01)
        TIM16->SR = 0x00;        //bajar flag
}


void TIM_16_Init (void)    //cada tick 100us
{
    if (!RCC_TIM16_CLK)
        RCC_TIM16_CLK_ON;

    //Configuracion del timer.
    TIM16->CR1 = 0x00;		//clk int / 1; upcounting; uev
    TIM16->PSC = 4799;			//tick 100us
    TIM16->EGR |= TIM_EGR_UG;    //update para cargar preescaler
    TIM16->ARR = 0xFFFF;
    TIM16->CNT = 0;

    TIM16->CR1 |= TIM_CR1_CEN;
}

void OneShootTIM16 (unsigned short a)
{
	TIM16->ARR = a;
	TIM16->CR1 |= TIM_CR1_CEN;
}

void TIM16Enable (void)
{
	TIM16->CR1 |= TIM_CR1_CEN;
}

void TIM16Disable (void)
{
	TIM16->CR1 &= ~TIM_CR1_CEN;
}


void TIM17_IRQHandler (void)	//200uS
{
	if (TIM17->SR & 0x01)
		TIM17->SR = 0x00;		//bajar flag
}


void TIM_17_Init (void)
{
	if (!RCC_TIM17_CLK)
		RCC_TIM17_CLK_ON;

	//Configuracion del timer.
	TIM17->ARR = 400;		//400us
	TIM17->CNT = 0;
	TIM17->PSC = 47;

	// Enable timer interrupt ver UDIS
	TIM17->DIER |= TIM_DIER_UIE;
	TIM17->CR1 |= TIM_CR1_URS | TIM_CR1_CEN;	//solo int cuando hay overflow y one shot

	NVIC_EnableIRQ(TIM17_IRQn);
	NVIC_SetPriority(TIM17_IRQn, 8);
}

//--- end of file ---//
