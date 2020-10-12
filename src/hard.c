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

// Externals -------------------------------------------------------------------
extern volatile unsigned short timer_led;


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
    
}

//--- end of file ---//
