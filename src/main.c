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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gpio.h"
#include "tim.h"
#include "uart.h"
#include "hard.h"

#include "signals.h"

#include "core_cm0.h"
#include "adc.h"
#include "dma.h"
#include "flash_program.h"

#include "it.h"



//--- VARIABLES EXTERNAS ---//
// ------- Externals de Timers  ------
volatile unsigned short timer_signals = 0;
volatile unsigned short timer_led = 0;


// ------- Externals del Puerto serie  -------
// volatile unsigned char tx1buff[SIZEOF_DATA];
// volatile unsigned char rx1buff[SIZEOF_DATA];
volatile unsigned char usart1_have_data = 0;

//de signals y usart para sync
volatile unsigned char sync_on_signal = 0;

// ------- Externals del o para el ADC -------
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

//--- VARIABLES GLOBALES ---//
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




//--- FUNCIONES DEL MODULO ---//
void TimingDelay_Decrement(void);

// ------- para el LM311 -------
// extern void EXTI0_1_IRQHandler(void);
extern void EXTI2_3_IRQHandler(void);



//--- Private Definitions ---//


//-------------------------------------------//
// @brief  Main program.
// @param  None
// @retval None
//------------------------------------------//
int main(void)
{
    unsigned char i, ii;

    // main_state_t main_state = MAIN_INIT;

    // char s_lcd [100];

    //GPIO Configuration.
    GPIO_Config();

    //ACTIVAR SYSTICK TIMER
    if (SysTick_Config(48000))
    {
        while (1)	/* Capture error */
        {
            if (LED)
                LED_OFF;
            else
                LED_ON;

            for (i = 0; i < 255; i++)
            {
                asm (	"nop \n\t"
                        "nop \n\t"
                        "nop \n\t" );
            }
        }
    }

    //--- Prueba LED y JUMPER ---//
    // while (1)
    // {
    // 	if (STOP_JUMPER)
    // 	{
    // 		LED_OFF;
    // 	}
    // 	else
    // 	{
    // 		if (LED)
    // 			LED_OFF;
    // 		else
    // 		  	LED_ON;
    //
    // 		Wait_ms (250);
    // 	}
    // }
    //--- Fin Prueba LED y JUMPER ---//



    //--- Welcome code ---//
    LED_OFF;

    USART1Config();

    //--- Prueba LED y USART TX ---//
    // while (1)
    // {
    // 	Usart1Send((char *) (const char *) "ADC Sync getted!\r\n");
    // 	if (LED)
    // 		LED_OFF;
    // 	else
    // 		LED_ON;
    //
    // 	Wait_ms (1000);
    // }
    //--- Fin Prueba LED y USART TX---//

    //--- Prueba TIM16 ---//    
    // TIM_16_Init ();    //lo uso para los tiempos muertos entre las funciones de generacion de seniales

    // TIM16->CNT = 0;
    // while (1)
    // {
    //     if ((TIM16->CNT) > 10)
    //     {
    //         if (LED)
    //             LED_OFF;
    //         else
    //             LED_ON;

    //         TIM16->CNT = 0;
    //     }
    // }
    //--- Prueba TIM16 ---//    
    
    //--- Prueba LED y USART RX ---//
    // while (1)
    // {
    // 	if (usart1_have_data)
    // 	{
    // 		LED_ON;
    // 		usart1_have_data = 0;
    // 		ReadUsart1Buffer (s_lcd, SIZEOF_DATA);
    // 		Usart1Send(s_lcd);
    // 		LED_OFF;
    // 	}
    // }
    //--- Fin Prueba LED y USART RX ---//

//---------- Pruebas de Hardware --------//

    TIM_1_Init ();    //lo utilizo para synchro ADC muestras 1500Hz
    TIM_3_Init ();    //lo utilizo para mosfets LOW_LEFT, HIGH_LEFT, LOW_RIGHT, HIGH_RIGHT

    TIM_16_Init ();    //lo uso para los tiempos muertos entre las funciones de generacion de seniales

    //Update_TIM3_CH2 (10);
    // TIM3->CCR3 = 1000;
    // TIM3->ARR = 6858;

    // while (1);

    //--- Prueba Pines PWM ---//
    // EXTIOff ();
    // while (1)
    // {
    // 	for (d = 0; d < DUTY_100_PERCENT; d++)
    // 	{
    // 		Update_TIM3_CH1 (d);
    // 		Update_TIM3_CH2 (d);
    // 		Update_TIM3_CH3 (d);
    // 		Update_TIM3_CH4 (d);
    //
    // 		Wait_ms(2);
    // 	}
    // }
    //--- Fin Prueba Pines PWM ---//

    //--- Prueba de seniales PWM ---//
    //CUADRADA baja
    // LOW_RIGHT_PWM(DUTY_100_PERCENT+1);
    // HIGH_RIGHT_PWM(0);
    //
    // while (1)
    // {
    // 	HIGH_LEFT_PWM(0);
    // 	LOW_LEFT_PWM(DUTY_50_PERCENT);
    //
    // 	Wait_ms(20);
    //
    // 	LOW_LEFT_PWM(0);
    //
    // 	Wait_ms(20);
    // }

    // //CUADRADA alta derecha
    // while (1)
    // {
    // 	HIGH_LEFT_PWM(0);
    // 	LOW_LEFT_PWM(DUTY_100_PERCENT+1);
    //
    // 	LOW_RIGHT_PWM(0);
    // 	HIGH_RIGHT_PWM(DUTY_50_PERCENT);
    //
    // 	Wait_ms(20);
    //
    // 	HIGH_RIGHT_PWM(0);
    // 	Wait_ms(20);
    // }

    //Activo el ADC con DMA
    AdcConfig();

    //-- DMA configuration.
    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    ADC1->CR |= ADC_CR_ADSTART;

    for (i = 0; i < (3 * OWN_CHANNEL); i++)
    {
        LED_ON;
        Wait_ms(200);
        LED_OFF;
        Wait_ms(200);
    }
    Wait_ms(2000);
        


    //--- Pruebas lazo PID
    //-- primero preparo el puente H segun la funcion que busque
    LOW_LEFT_PWM(0);
    HIGH_RIGHT_PWM(0);
    LOW_RIGHT_PWM(DUTY_100_PERCENT+1);

    HIGH_LEFT_PWM(0);	//este es el que actua


#ifndef INT_SPEED_RESPONSE
    //prueba de nuevas rutinas
    SetOwnChannel (OWN_CHANNEL);
    SetSignalTypeAndOffset (SINUSOIDAL_SIGNAL, ZERO_DEG_OFFSET);
    SetFrequency (30, 0);
    SetPower (50);
#endif

#ifdef INT_SPEED_RESPONSE
    //prueba de nuevas rutinas
    // SetOwnChannel (OWN_CHANNEL);
    // SetSignalType (SINUSOIDAL_SIGNAL);
    // SetFrequency (THIRTY_HZ);
    // SetPower (100);

    SetOwnChannel (OWN_CHANNEL);
    SetSignalTypeAndOffset (TRIANGULAR_SIGNAL, ZERO_DEG_OFFSET);
    SetFrequency (TEN_HZ);
    SetPower (100);
#endif

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

#ifdef INT_SPEED_RESPONSE
    while (1)
    {
        //solo genera la senial seteada mas arriba y en algun punto de la misma corta por int
        TreatmentManager_IntSpeed ();

    }
#endif

#ifdef INT_WITH_LED
    while (1)
    {
        //no genero nada, solo espero int y muevo el LED        
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
#ifndef INT_WITH_LED
        //para pruebas
        // if (LED)
        // 	LED_OFF;
        // else
        LED_ON;
#endif

        // if (int_counter < 100)
        // 	int_counter++;
        // else
        // {
        // 	int_counter = 0;
        // 	Usart1Send("100 ints\n");
        // }


        Overcurrent_Shutdown();

        EXTI->PR |= 0x00000004;
    }
}


//--- end of file ---//
