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

//--- end of file ---//
