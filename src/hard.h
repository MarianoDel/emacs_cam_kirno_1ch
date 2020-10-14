//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### HARD.H ################################
//---------------------------------------------
#ifndef _HARD_H_
#define _HARD_H_


//----------- Defines For Configuration -------------

//----------- Hardware Board Version -------------
#define HARD_VER_1_0    //first board manufacturer

//----------- Software Version -------------------
#define SOFT_VER_1_0    //first program release


//-------- Max Current Allowed on Bobins ----------------
// #define MAX_CURRENT_3_0_A
#define MAX_CURRENT_3_6_A

//-------- Type of Program ----------------
#define SYSTEM_AUTONOMOUS
// #define SYSTEM_WITH_MANAGEMENT


//-------- Type of Program and Features ----------------
//Si utiliza la proteccion con la int para cortar la corriente
#define USE_PROTECTION_WITH_INT

//Si utiliza la proteccion de soft overcurrent o no
#define USE_SOFT_OVERCURRENT

//Si utiliza la proteccion de no current
#define USE_SOFT_NO_CURRENT

//El synchro viene por puerto serie o lo genero internamente
// #define SYNC_EXTERNAL_OVER_USART
#define SYNC_INTERNAL_IN_TIM1

//el LED lo uso para debug de varios procesos, ver mas abajo cuales
// #define USE_LED_FOR_DEBUG
// #define USE_LED_IN_TREATMENT_MANAGER
#define USE_LED_IN_SYSTEM_AUTONOMOUS

//-------- Kind of Reports Sended ----------------

//-------- Others Configurations depending on the formers ------------
#ifdef USE_SOFT_OVERCURRENT
// #define USE_SOFT_OVERCURRENT_WITH_PERCENTAGE
#define USE_SOFT_OVERCURRENT_WITH_ABSOLUTE_VALUE

#ifdef USE_SOFT_OVERCURRENT_WITH_PERCENTAGE
//porcentaje de corriente para SOFT OVERCURRENT, 10 -> 100% 15 -> 150%
#define SIGNAL_ADMITED_WITH_OVERCURRENT    15
#endif
#ifdef USE_SOFT_OVERCURRENT_WITH_ABSOLUTE_VALUE
//adicional de corriente ADC 10bits 155 puntos -> 1A
#define SIGNAL_ADMITED_WITH_OVERCURRENT    155
#endif
#endif

//quantity of signals before anounce the no current error
#ifdef USE_SOFT_NO_CURRENT
#define SIGNALS_ADMITED_WITH_NO_CURRENT    5    
#endif

#ifdef USE_LED_FOR_DEBUG
// #define LED_SHOW_SEQUENCE
#define LED_SHOW_MSGS
// #define LED_SHOW_INT
// #define LED_SHOW_SYNC_UART
// #define LED_SHOW_SYNC_SIGNAL
// #define LED_SHOW_SYNC_SIGNAL_ON_TIM1
#endif

//-------- Hysteresis Conf ------------------------

//-------- PWM Conf ------------------------

//-------- Usart Conf ----------------------
// #define USART_TX_OUTPUT_OPEN_DRAIN
// #define USART_RX_PULLUP

//-------- End Of Defines For Configuration ------

//-------- Anouncemets ---------------------------
#ifdef HARD_VER_1_2
#define HARD_ANNOUNCEMENT "Hardware V: 1.2\r\n"
#endif

#ifdef HARD_VER_1_1
#define HARD_ANNOUNCEMENT "Hardware V: 1.1\r\n"
#endif

#ifdef HARD_VER_1_0
#define HARD_ANNOUNCEMENT "Hardware V: 1.0\r\n"
#endif

#ifdef SOFT_VER_1_2
#define SOFT_ANNOUNCEMENT "Software V: 1.2\r\n"
#endif

#ifdef SOFT_VER_1_1
#define SOFT_ANNOUNCEMENT "Software V: 1.1\r\n"
#endif

#ifdef SOFT_VER_1_0
#define SOFT_ANNOUNCEMENT "Software V: 1.0\r\n"
#endif


//--- Gpios Configuration ----------------------------------
//GPIOA pin0
#define BUZZER    ((GPIOA->ODR & 0x0001) != 0)
#define BUZZER_ON    (GPIOA->BSRR = 0x00000001)
#define BUZZER_OFF    (GPIOA->BSRR = 0x00010000)

//GPIOA pin1    ADC_Channel_1 I_Sense

//GPIOA pin2	PROT Input
#define PROT    ((GPIOA->IDR & 0x0004) != 0)

//GPIOA pin3	ADC_Channel_3 I_Sense_Neg

//GPIOA pin4	NC
//GPIOA pin5	NC
//GPIOA pin6    NC

//GPIOA pin7    TIM3_CH2 HIGH_LEFT
//GPIOB pin0    TIM3_CH3 LOW_RIGHT

//GPIOB pin1    NC
//GPIOA pin8	NC

//GPIOA pin9	usart1 tx
//GPIOA pin10	usart1 rx

//GPIOA pin11	NC

//GPIOA pin12
#define LED    ((GPIOA->ODR & 0x1000) != 0)
#define LED_ON    (GPIOA->BSRR = 0x00001000)
#define LED_OFF    (GPIOA->BSRR = 0x10000000)

//GPIOA pin13	NC
//GPIOA pin14	NC

//GPIOA pin15    ON_TREATMENT
#define ON_TREATMENT    ((GPIOA->IDR & 0x8000) == 0)

//GPIOB pin3    Config Input - NC
//GPIOB pin4    Config Input - NC

//GPIOB pin5	NC

//GPIOB pin6
#define STOP_JUMPER ((GPIOB->IDR & 0x0040) == 0)

//GPIOB pin7	NC

//--- End of Gpios Configuration ---------------------------

// Main Program States
typedef enum
{
    MAIN_INIT = 0,
    SYNCHRO_ADC,
    SET_ZERO_CURRENT,
    MAIN_OVERCURRENT,
    SET_COUNTERS_AND_PHONE,
    LAMP_OFF,
    START_GSM,
    CONFIG_GSM,
    WELCOME_GSM,
    LAMP_ON,
    GO_TO_MAINS_FAILURE,
    MAINS_FAILURE

} main_state_t;


//Estados Externos de LED BLINKING
#define LED_NO_BLINKING    0
#define LED_TREATMENT_STANDBY    1
#define LED_TREATMENT_GENERATING    2
#define LED_TREATMENT_JUMPER_PROTECTED    5


#define LED_TOGGLE    do { \
    if (LED) \
        LED_OFF; \
    else \
        LED_ON; \
    } while (0)


typedef enum {
    SW_NO = 0,
    SW_MIN,
    SW_HALF,
    SW_FULL
    
} resp_sw_t;

// Buzzer Commands
#define BUZZER_STOP_CMD        0
#define BUZZER_LONG_CMD        1
#define BUZZER_HALF_CMD        2
#define BUZZER_SHORT_CMD        3
#define BUZZER_MULTI_CMD        4

// Buzzer Timeouts
#define TT_BUZZER_BIP_SHORT		300
#define TT_BUZZER_BIP_SHORT_WAIT	500
#define TT_BUZZER_BIP_HALF		600
#define TT_BUZZER_BIP_HALF_WAIT    	800
#define TT_BUZZER_BIP_LONG		2000
#define TT_BUZZER_BIP_LONG_WAIT        2000


//--- Stringtify Utils -----------------------
#define STRING_CONCAT(str1,str2) #str1 " " #str2
#define STRING_CONCAT_NEW_LINE(str1,str2) xstr(str1) #str2 "\n"
#define xstr_macro(s) str_macro(s)
#define str_macro(s) #s

// Module Functions ----------------------------------------------------------
void ChangeLed (unsigned char);
void UpdateLed (void);
void WelcomeCode (void);

void UpdateBuzzer (void);
void UpdateSwitches (void);
void BuzzerCommands(unsigned char, unsigned char);
resp_sw_t CheckS1 (void);

#endif /* _HARD_H_ */

//--- end of file ---//

