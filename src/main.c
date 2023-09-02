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

// -- Externals para synchro en Usart o Tim1  ------
volatile unsigned char sync_on_signal = 0;

// -- Externals de Tests  ------
volatile unsigned char interrupt_getted = 0;    //only for tests

// -- Externals del o para el ADC -------
volatile unsigned short adc_ch[ADC_CHANNEL_QUANTITY];
volatile unsigned char seq_ready = 0;

// ------- Externals del PID dsp.c -------
unsigned short pid_param_p = 0;
unsigned short pid_param_i = 0;
unsigned short pid_param_d = 0;


#ifdef SYSTEM_AUTONOMOUS
volatile unsigned short buzzer_timeout = 0;
volatile unsigned char switches_timeout = 0;
#endif 

// parameters_typedef param_struct;

// Globals ---------------------------------------------------------------------
volatile unsigned char int_counter = 0;

// ------- de los timers -------
volatile unsigned short wait_ms_var = 0;
volatile unsigned short timer_standby;

volatile unsigned short secs = 0;
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
    Wait_ms(100);
    SetOwnChannel(1);
    Usart1Send("\nOwn Channel ch1\n");
    Wait_ms(100);
    Usart1Send("\r\nStarting Main Program...\r\n");

#ifdef SYSTEM_AUTONOMOUS
    unsigned char button_state = 0;
    error_t errors = ERROR_OK;
    // char buff [100] = { 0 };
    
    while (1)
    {
        //este es el programa principal, maneja a GenerateSignal()
        TreatmentManager ();

        //Cosas que no dependen del estado del programa
        UpdateCommunications();

        UpdateLed();
        UpdateBuzzer();
        UpdateSwitches();

        //Manejar TreatmentManager con el boton
        switch (button_state)
        {
        case 0:
            //comienzo el programa
            BuzzerCommands(BUZZER_SHORT_CMD, 2);
            Usart1Send("Main Standby!\n");
#ifdef USE_LED_IN_SYSTEM_AUTONOMOUS
            ChangeLed(LED_TREATMENT_STANDBY);
#endif
            button_state++;
            break;

        case 1:
            //comienzo tratamiento?
            if (CheckS1() > SW_MIN)
            {
                BuzzerCommands(BUZZER_HALF_CMD, 1);
                SetSignalTypeAndOffset (AUTONOMOUS_SIGNAL, ZERO_DEG_OFFSET);
                SetFrequency (AUTONOMOUS_FREQ_INT, AUTONOMOUS_FREQ_DEC);
                SetPower (AUTONOMOUS_POWER);
                
                button_state++;
            }
            break;

        case 2:
            if (CheckS1() == SW_NO)
            {
                //comenzar tratamiento ahora
                Usart1Send("Comenzar tratamiento ahora!\n");
                // SetSignalTypeAndOffset (SINUSOIDAL_SIGNAL, ZERO_DEG_OFFSET);
                // SetFrequency (10, 00);
                // SetPower (100);
#ifdef USE_LED_IN_SYSTEM_AUTONOMOUS
                ChangeLed(LED_TREATMENT_GENERATING);
#endif
                minutes = AUTONOMOUS_TIME;
                if (StartTreatment() == resp_ok)
                    button_state++;
                else
                {
                    button_state = 1;
                    BuzzerCommands(BUZZER_LONG_CMD, 1);
                    Usart1Send("Errores al querer comenzar tratamiento\n");
                }
            }
            break;
            
        case 3:
            //generando reviso si termina o me piden que corte
            //cortar generacion
            if (CheckS1() > SW_NO)
            {
                BuzzerCommands(BUZZER_SHORT_CMD, 3);
                StopTreatment();
                button_state++;
            }

            //termino el tiempo del tratamiento
            if (!minutes)
            {
#ifdef USE_TREATMENT_COMBINED
                Usart1Send("Termino etapa por tiempo\n");
                BuzzerCommands(BUZZER_SHORT_CMD, 1);
                StopTreatment();
                timer_standby = 2000;
                button_state = 5;
#else
                Usart1Send("Termino tratamiento por tiempo\n");
                BuzzerCommands(BUZZER_SHORT_CMD, 3);
                StopTreatment();
#ifdef USE_LED_IN_SYSTEM_AUTONOMOUS
                ChangeLed(LED_TREATMENT_STANDBY);
#endif
                button_state = 1;
#endif
            }

            errors = GetErrorStatus();
            if (errors != ERROR_OK)
            {
                char buff [100] = { 0 };
                sprintf (buff, "treat err, ch1: 0x%04x\r\n", errors);
                Usart1Send(buff);
                BuzzerCommands(BUZZER_LONG_CMD, 1);
#ifdef USE_LED_IN_SYSTEM_AUTONOMOUS
                ChangeLed(LED_TREATMENT_JUMPER_PROTECTED);
#endif
                button_state = 1;
            }
            break;
            
        case 4:
            //termino tratamiento
            if (CheckS1() == SW_NO)
            {
                Usart1Send("Termino tratamiento por el usuario\n");
#ifdef USE_LED_IN_SYSTEM_AUTONOMOUS
                ChangeLed(LED_TREATMENT_STANDBY);
#endif
                button_state = 1;
            }
            break;

        case 5:    // change treatment on the fly
            //termino tratamiento
            if (!timer_standby)
            {
                BuzzerCommands(BUZZER_HALF_CMD, 1);
                SetSignalTypeAndOffset (AUTONOMOUS_SIGNAL_COMBINED, ZERO_DEG_OFFSET);
                SetFrequency (AUTONOMOUS_FREQ_INT_COMBINED, AUTONOMOUS_FREQ_DEC_COMBINED);
                SetPower (AUTONOMOUS_POWER_COMBINED);                
                minutes = AUTONOMOUS_TIME_COMBINED;
                
                if (StartTreatment() == resp_ok)
                    button_state = 6;
                else
                {
                    button_state = 1;
                    BuzzerCommands(BUZZER_LONG_CMD, 1);
                    Usart1Send("Errores al querer comenzar tratamiento\n");
                }
            }
            break;

        case 6:
            //generando reviso si termina o me piden que corte
            //cortar generacion
            if (CheckS1() > SW_NO)
            {
                BuzzerCommands(BUZZER_SHORT_CMD, 3);
                StopTreatment();
                button_state++;
            }

            //termino el tiempo del tratamiento
            if (!minutes)
            {
                Usart1Send("Termino tratamiento por tiempo\n");
                BuzzerCommands(BUZZER_SHORT_CMD, 3);
                StopTreatment();
#ifdef USE_LED_IN_SYSTEM_AUTONOMOUS
                ChangeLed(LED_TREATMENT_STANDBY);
#endif
                button_state = 1;
            }

            errors = GetErrorStatus();
            if (errors != ERROR_OK)
            {
                char buff [100] = { 0 };
                sprintf (buff, "treat err, ch1: 0x%04x\r\n", errors);
                Usart1Send(buff);
                BuzzerCommands(BUZZER_LONG_CMD, 1);
#ifdef USE_LED_IN_SYSTEM_AUTONOMOUS
                ChangeLed(LED_TREATMENT_JUMPER_PROTECTED);
#endif
                button_state = 1;
            }
            break;
            
        default:
            button_state = 0;
            break;
        }
    }
#endif

#ifdef SYSTEM_WITH_MANAGEMENT
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

    if (timer_signals)
        timer_signals--;

    if (timer_led)
        timer_led--;
    
#ifdef SYSTEM_AUTONOMOUS
    if (secs > 59999)
    {
        if (minutes)
            minutes--;

        secs = 0;
    }
    else
        secs++;
    
    if (buzzer_timeout)
        buzzer_timeout--;

    if (switches_timeout)
        switches_timeout--;
        
#endif

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
