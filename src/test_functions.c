//------------------------------------------------
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.C #########################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "test_functions.h"
#include "stm32f0xx.h"
#include "hard.h"
#include "tim.h"
#include "usart.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"

#include <stdio.h>
#include <string.h>



// Externals -------------------------------------------------------------------
extern volatile unsigned char usart1_have_data;
extern volatile unsigned char adc_int_seq_ready;
extern volatile unsigned short adc_ch [];

// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void TF_Led (void)
{
    while (1)
    {
        if (LED)
            LED_OFF;
        else
            LED_ON;

        Wait_ms(300);
    }
}


void TF_Led_Blinking (void)
{
    ChangeLed(LED_TREATMENT_GENERATING);
    while (1)
        UpdateLed();

}


void TF_Buzzer (void)
{
    while (1)
    {
        Wait_ms(2000);
        BUZZER_ON;
        Wait_ms(200);
        BUZZER_OFF;
    }
}


void TF_Gpio_Input_Prot (void)
{
    while (1)
    {
        if (PROT)
            LED_ON;
        else
            LED_OFF;
    }
}


void TF_Gpio_Input_Stop_Jumper (void)
{
    while (1)
    {
        if (STOP_JUMPER)
            LED_ON;
        else
            LED_OFF;
    }
}


void TF_Gpio_Input_On_Treatment (void)
{
    while (1)
    {
        if (ON_TREATMENT)
            LED_ON;
        else
            LED_OFF;
    }
}


void TF_Usart1_TxRx (void)
{
    for (unsigned char i = 0; i < 5; i++)
    {
        LED_ON;
        Wait_ms(250);
        LED_OFF;
        Wait_ms(250);
    }
    
    Usart1Config();

    char s_to_send [100] = { 0 };
    Usart1Send("Ready to test...\n");
    while (1)
    {
        if (usart1_have_data)
        {
            usart1_have_data = 0;
            
            if (LED)
                LED_OFF;
            else
                LED_ON;
            
            Usart1ReadBuffer((unsigned char *) s_to_send, 100);
            Wait_ms(1000);
            Usart1Send(s_to_send);
        }
    }
}


void TF_Usart1_Adc_Dma (void)
{
    for (unsigned char i = 0; i < 5; i++)
    {
        LED_ON;
        Wait_ms(250);
        LED_OFF;
        Wait_ms(250);
    }
    
    Usart1Config();

    //-- ADC Init
    AdcConfig();

    //-- DMA configuration and Init
    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    ADC1->CR |= ADC_CR_ADSTART;

    unsigned short cntr = 0;
    char s_to_send [100] = { 0 };
    Usart1Send("\nTesting ADC with dma transfers...\n");

    while (1)
    {
        if (sequence_ready)
        {
            sequence_ready_reset;
            if (cntr < 10000)
                cntr++;
            else
            {
                sprintf(s_to_send, "I_Sense: %d I_Sense_Neg: %d\n",
                        I_Sense,
                        I_Sense_Neg);
                
                Usart1Send(s_to_send);
                cntr = 0;
            }
        }            
    }
}


void TF_Tim3_Ch2_Pwm (void)
{
    TIM_3_Init ();    //lo utilizo para mosfets TIM3_CH2->HIGH_LEFT, TIM3_CH3->LOW_RIGHT

    HIGH_LEFT(DUTY_50_PERCENT);

    while (1);
}


void TF_Tim3_Ch3_Pwm (void)
{
    TIM_3_Init ();    //lo utilizo para mosfets TIM3_CH2->HIGH_LEFT, TIM3_CH3->LOW_RIGHT

    LOW_RIGHT(DUTY_50_PERCENT);

    while (1);
}




//--- end of file ---//
