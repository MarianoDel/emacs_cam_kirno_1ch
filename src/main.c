//------------------------------------------------
// #### PROYECTO STRETCHER F030 - Power Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MAIN.C ###################################
//------------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "hard.h"
#include "gpio.h"
#include "test_functions.h"
#include "tim.h"
#include "usart.h"
#include "signals.h"
#include "core_cm0.h"
#include "adc.h"
#include "dma.h"
#include "flash_program.h"
#include "it.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// Externals -------------------------------------------------------------------
// -- Externals de Timers  ------
volatile unsigned short timer_signals = 0;
volatile unsigned short timer_led = 0;

// -- Externals de Tests  ------
volatile unsigned char interrupt_getted = 0;    //only for tests

// -- Externals del o para el ADC -------
volatile unsigned short adc_ch[ADC_CHANNEL_QUANTITY];
volatile unsigned char seq_ready = 0;

// ------- Externals del PID dsp.c -------
unsigned short pid_param_p = 0;
unsigned short pid_param_i = 0;
unsigned short pid_param_d = 0;

// ------- Externals para filtros -------
unsigned short mains_voltage_filtered;
//
//
// volatile unsigned short scroll1_timer = 0;
// volatile unsigned short scroll2_timer = 0;
//
// volatile unsigned short standalone_timer;
// volatile unsigned short standalone_enable_menu_timer;
// //volatile unsigned short standalone_menu_timer;
// volatile unsigned char grouped_master_timeout_timer;
volatile unsigned short take_temp_sample = 0;


// parameters_typedef param_struct;

// Globals ---------------------------------------------------------------------
volatile unsigned char int_counter = 0;

// ------- de los timers -------
volatile unsigned short wait_ms_var = 0;
volatile unsigned short timer_standby;
//volatile unsigned char display_timer;
volatile unsigned char timer_meas;

volatile unsigned char door_filter;
volatile unsigned char take_sample;
volatile unsigned char move_relay;


volatile unsigned short secs = 0;
volatile unsigned char hours = 0;
volatile unsigned char minutes = 0;


// ------- para el LM311 -------
// extern void EXTI0_1_IRQHandler(void);
extern void EXTI2_3_IRQHandler(void);


// Module Private Functions ----------------------------------------------------
void TimingDelay_Decrement(void);
void SysTickError (void);


//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
    //GPIO Configuration.
    GPIO_Config();

    //Start the SysTick Timer
    if (SysTick_Config(48000))
        SysTickError();

    
    //--- Hard Test Functions ---//
    // TF_Led ();

    // TF_Led_Blinking ();

    // TF_Buzzer ();

    // TF_Gpio_Input_Prot ();
    
    // TF_Gpio_Input_Stop_Jumper ();
    
    // TF_Gpio_Input_On_Treatment ();
    
    // TF_Tim3_Ch2_Pwm ();
    
    // TF_Tim3_Ch3_Pwm ();

    // TF_Usart1_Single ();
    
    // TF_Usart1_Multiple ();
    
    // TF_Usart1_TxRx ();
    
    // TF_Usart1_Adc ();
    
    // TF_Usart1_Adc_Dma ();

    // TF_Gpio_Int_Usart1 ();
    
    //--- End of Hard Test Functions ---//


    //--- Production Program ---//
    LED_OFF;
    Usart1Config();

    TIM_1_Init ();    //lo utilizo para synchro ADC muestras 1500Hz
    TIM_3_Init ();    //lo utilizo para mosfets TIM3_CH2->HIGH_LEFT, TIM3_CH3->LOW_RIGHT
    HIGH_LEFT(DUTY_NONE);
    LOW_RIGHT(DUTY_NONE);

    TIM_16_Init ();    //lo uso para los tiempos muertos entre las funciones de generacion de seniales

    //-- ADC and DMA configuration
    AdcConfig();

    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    ADC1->CR |= ADC_CR_ADSTART;

    //--- Welcome code ---//
    WelcomeCode();
    Usart1Send("\r\nStarting Main Program...\r\n");
    while(1);
    
#ifdef ONLY_POWER_WITHOUT_MANAGEMENT
    while (1)
    {
        //Cosas que dependen de las muestras
        GenerateSignal();
    }
#endif

#ifdef POWER_WITH_MANAGEMENT
    while (1)
    {
        //este es el programa principal, maneja a GenerateSignal()
        TreatmentManager ();

        //Cosas que no dependen del estado del programa
        UpdateCommunications();

        UpdateLed();

    }
#endif

    return 0;
}
//--- End of Main ---//



void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    if (take_temp_sample)
        take_temp_sample--;

    if (timer_meas)
        timer_meas--;

    if (timer_signals)
        timer_signals--;

    if (timer_led)
        timer_led--;
    // //cuenta de a 1 minuto
    // if (secs > 59999)	//pasaron 1 min
    // {
    // 	minutes++;
    // 	secs = 0;
    // }
    // else
    // 	secs++;
    //
    // if (minutes > 60)
    // {
    // 	hours++;
    // 	minutes = 0;
    // }


}


void EXTI2_3_IRQHandler(void)
{
    if(EXTI->PR & 0x00000004)	//Line2
    {
        Overcurrent_Shutdown();
        EXTI->PR |= 0x00000004;
        interrupt_getted = 1;    //only for tests
    }
}


void SysTickError (void)
{
    //Capture systick error...
    while (1)
    {
        if (LED)
            LED_OFF;
        else
            LED_ON;

        for (unsigned char i = 0; i < 255; i++)
        {
            asm ("nop \n\t"
                 "nop \n\t"
                 "nop \n\t" );
        }
    }
}

//--- end of file ---//
