//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### HARD.C ################################
//---------------------------------------------
#include "hard.h"
#include "stm32f0xx.h"
#include "tim.h"
#include "usart.h"

#include <stdio.h>

// Private Types Macros and Constants ------------------------------------------
// Led States
typedef enum
{    
    START_BLINKING = 0,
    WAIT_TO_OFF,
    WAIT_TO_ON,
    WAIT_NEW_CYCLE
} led_state_t;


#define LED_BLINKING_ON    LED_ON
#define LED_BLINGING_OFF    LED_OFF
#define TT_LED_IN_ON_OFF    200
#define TT_LED_WAIT_IN_OFF    2000

// Buzzer Bips States
typedef enum
{    
    BUZZER_WAIT_COMMANDS = 0,
    BUZZER_MARK,
    BUZZER_SPACE,
    BUZZER_TO_STOP
    
} buzzer_state_t;


// Externals -------------------------------------------------------------------
extern volatile unsigned short timer_led;

#ifdef SYSTEM_AUTONOMOUS
extern volatile unsigned short buzzer_timeout;
extern volatile unsigned char switches_timeout;
#endif 


// Globals ---------------------------------------------------------------------
led_state_t led_state = START_BLINKING;
unsigned char blink = 0;
unsigned char how_many_blinks = 0;


// Module Private Functions ----------------------------------------------------

// Module Functions ------------------------------------------------------------
//cambia configuracion de bips del LED
void ChangeLed (unsigned char how_many)
{
    how_many_blinks = how_many;
    led_state = START_BLINKING;
}

//mueve el LED segun el estado del Pote
void UpdateLed (void)
{
#ifndef USE_LED_FOR_DEBUG
    switch (led_state)
    {
        case START_BLINKING:
            blink = how_many_blinks;
            
            if (blink)
                led_state = WAIT_TO_ON;

            break;

        case WAIT_TO_OFF:
            if (!timer_led)
            {
                LED_BLINGING_OFF;
                timer_led = TT_LED_IN_ON_OFF;
                led_state++;
            }
            break;

        case WAIT_TO_ON:
            if (!timer_led)
            {
                if (blink)
                {
                    blink--;
                    timer_led = TT_LED_IN_ON_OFF;
                    led_state = WAIT_TO_OFF;
                    LED_BLINKING_ON;
                }
                else
                {
                    led_state = WAIT_NEW_CYCLE;
                    timer_led = TT_LED_WAIT_IN_OFF;
                }
            }
            break;

        case WAIT_NEW_CYCLE:
            if (!timer_led)
                led_state = START_BLINKING;

            break;

        default:
            led_state = START_BLINKING;
            break;
    }
#endif
}

void WelcomeCode (void)
{
    char str [128] = { 0 };
    
    Usart1Send("\r\nKirno -- Camilla One Channel --\r\n");

#ifdef HARD_ANNOUNCEMENT
    Usart1Send(HARD_ANNOUNCEMENT);
    Wait_ms(100);    
#else
#error	"No Hardware defined in hard.h file"
#endif

#ifdef SOFT_ANNOUNCEMENT
    Usart1Send(SOFT_ANNOUNCEMENT);
    Wait_ms(100);    
#else
#error	"No Soft Version defined in hard.h file"
#endif
    
    // Main Program Type
    Usart1Send("\r\nProgram Type:\n");
    Wait_ms(100);
    
#ifdef SYSTEM_AUTONOMOUS
    sprintf(str,"[%s] %s\n", __FILE__, str_macro(SYSTEM_AUTONOMOUS));
    Usart1Send(str);
    Wait_ms(40);
#endif

#ifdef SYSTEM_WITH_MANAGEMENT
    sprintf(str,"[%s] %s\n", __FILE__, str_macro(SYSTEM_WITH_MANAGEMENT));
    Usart1Send(str);
    Wait_ms(40);
#endif

    // Program Features
    Usart1Send("\r\nFeatures:\n");
    Wait_ms(100);
    
#ifdef USE_PROTECTION_WITH_INT
    sprintf(str,"[%s] %s\n", __FILE__, str_macro(USE_PROTECTION_WITH_INT));
    Usart1Send(str);
    Wait_ms(40);
#endif

#ifdef USE_SOFT_OVERCURRENT
    sprintf(str,"[%s] %s\n", __FILE__, str_macro(USE_SOFT_OVERCURRENT));
    Usart1Send(str);
    Wait_ms(40);
#endif

#ifdef USE_SOFT_NO_CURRENT
    sprintf(str,"[%s] %s\n", __FILE__, str_macro(USE_SOFT_NO_CURRENT));
    Usart1Send(str);
    Wait_ms(40);
#endif

#ifdef SYNC_EXTERNAL_OVER_USART
    sprintf(str,"[%s] %s\n", __FILE__, str_macro(SYNC_EXTERNAL_OVER_USART));
    Usart1Send(str);
    Wait_ms(40);
#endif
    
#ifdef SYNC_INTERNAL_IN_TIM1
    sprintf(str,"[%s] %s\n", __FILE__, str_macro(SYNC_INTERNAL_IN_TIM1));
    Usart1Send(str);
    Wait_ms(40);
#endif
    
    
}

#ifdef SYSTEM_AUTONOMOUS
// for the buzzer
buzzer_state_t buzzer_state = BUZZER_WAIT_COMMANDS;
unsigned char buzzer_multiple = 0;
unsigned short buzzer_timer_reload_mark = 0;
unsigned short buzzer_timer_reload_space = 0;

void BuzzerCommands(unsigned char command, unsigned char multiple)
{
    if (command == BUZZER_STOP_CMD)
        buzzer_state = BUZZER_TO_STOP;
    else
    {
        if (command == BUZZER_LONG_CMD)
        {
            buzzer_timer_reload_mark = TT_BUZZER_BIP_LONG;
            buzzer_timer_reload_space = TT_BUZZER_BIP_LONG_WAIT;
        }
        else if (command == BUZZER_HALF_CMD)
        {
            buzzer_timer_reload_mark = TT_BUZZER_BIP_HALF;
            buzzer_timer_reload_space = TT_BUZZER_BIP_HALF_WAIT;
        }
        else
        {
            buzzer_timer_reload_mark = TT_BUZZER_BIP_SHORT;
            buzzer_timer_reload_space = TT_BUZZER_BIP_SHORT_WAIT;
        }

        buzzer_state = BUZZER_MARK;
        buzzer_timeout = 0;
        buzzer_multiple = multiple;
    }
}


void UpdateBuzzer (void)
{
    switch (buzzer_state)
    {
        case BUZZER_WAIT_COMMANDS:
            break;

        case BUZZER_MARK:
            if (!buzzer_timeout)
            {
                BUZZER_ON;
                buzzer_state++;
                buzzer_timeout = buzzer_timer_reload_mark;
            }
            break;

        case BUZZER_SPACE:
            if (!buzzer_timeout)
            {
                if (buzzer_multiple > 1)
                {
                    buzzer_multiple--;
                    BUZZER_OFF;
                    buzzer_state = BUZZER_MARK;
                    buzzer_timeout = buzzer_timer_reload_space;
                }
                else
                    buzzer_state = BUZZER_TO_STOP;
            }
            break;

        case BUZZER_TO_STOP:
        default:
            BUZZER_OFF;
            buzzer_state = BUZZER_WAIT_COMMANDS;
            break;
    }
}


#define SWITCHES_TIMER_RELOAD    5
#define SWITCHES_THRESHOLD_FULL	1000		//5 segundos
#define SWITCHES_THRESHOLD_HALF	100		//1 segundo
#define SWITCHES_THRESHOLD_MIN	5		//25 ms

unsigned short s1 = 0;
resp_sw_t CheckS1 (void)	//cada check tiene SWITCHES_TIMER_RELOAD ms
{
    if (s1 > SWITCHES_THRESHOLD_FULL)
        return SW_FULL;

    if (s1 > SWITCHES_THRESHOLD_HALF)
        return SW_HALF;

    if (s1 > SWITCHES_THRESHOLD_MIN)
        return SW_MIN;

    return SW_NO;
}

#define S1_PIN    ON_TREATMENT
void UpdateSwitches (void)
{
    //revisa los switches cada 10ms
    if (!switches_timeout)
    {
        if (S1_PIN)
            s1++;
        else if (s1 > 50)
            s1 -= 50;
        else if (s1 > 10)
            s1 -= 5;
        else if (s1)
            s1--;
        
        switches_timeout = SWITCHES_TIMER_RELOAD;
    }
}
#endif    //SYSTEM_AUTONOMOUS

//--- end of file ---//
