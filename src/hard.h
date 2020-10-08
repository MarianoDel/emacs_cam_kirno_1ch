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
//----------- Harcode Direction -------------
// #define OWN_CHANNEL 1
// #define OWN_CHANNEL 2
#define OWN_CHANNEL 3

//----------- Hardware Board Version -------------
// #define VER_1_0
// #define VER_1_1		//mismo pinout que VER_1_0
#define VER_2_0		//micro en placa grande

//----------- Software Version -------------------
// #define SOFT_VER_1_2    //usa comandos para pantalla DWIN DGUS
                        //

#define SOFT_VER_1_1    //usa DMA en ADC, la senial es solo la parte util, el resto son timers
                        //para la frecuencia y pid mueve timers y velocidad de muestreo

// #define SOFT_VER_1_0    //este es el soft de las primeras placas, la senial la hacia con
                        //puntero y la frecuencia saltando con ese mismo puntero
                        //sin DMA para el ADC y seniales con 0V en tabla

#ifdef SOFT_VER_1_2
#define SOFT_ANNOUNCEMENT "Software V: 1.2"
#endif

#ifdef SOFT_VER_1_1
#define SOFT_ANNOUNCEMENT "Software V: 1.1"
#endif

#ifdef SOFT_VER_1_0
#define SOFT_ANNOUNCEMENT "Software V: 1.0"
#endif

//-------- Max Current Allowed on Bobins ----------------
// #define MAX_CURRENT_3_0_A
#define MAX_CURRENT_3_6_A

//-------- Type of Program ----------------
#define POWER_WITH_MANAGEMENT
// #define ONLY_POWER_WITHOUT_MANAGEMENT
// #define INT_SPEED_RESPONSE    //genera seniales y ve int, 30ms y reactiva solo
// #define INT_WITH_LED        //no genero nada, solo espero int y muevo el LED


//-------- Type of Program and Features ----------------
//Si utiliza la proteccion con la int para cortar la corriente
#define USE_PROTECTION_WITH_INT

//Si utiliza la proteccion de soft overcurrent o no
#define USE_SOFT_OVERCURRENT

//Si utiliza la proteccion de no current
#define USE_SOFT_NO_CURRENT

//Modo de uso de la USART (placa individual single - placa enganchada bus)
#define USART_IN_BUS
// #define USART_SINGLE

//el LED lo uso para debug de varios procesos, ver mas abajo cuales
// #define USE_LED_FOR_DEBUG

//-------- Kind of Reports Sended ----------------

//-------- Others Configurations depending on the formers ------------
#define SYNC_CHAR    '*'

#ifdef USART_IN_BUS
#define USART_TX_OUTPUT_OPEN_DRAIN
#endif


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

//cantidad de seniales que permito sin corriente antes de poner error
#ifdef USE_SOFT_NO_CURRENT
#define SIGNALS_ADMITED_WITH_NO_CURRENT    5    
#endif

#ifdef USE_LED_FOR_DEBUG
// #define LED_SHOW_SEQUENCE
// #define LED_SHOW_MSGS
// #define LED_SHOW_INT
// #define LED_SHOW_SYNC_UART
#define LED_SHOW_SYNC_SIGNAL
#endif

//-------- Hysteresis Conf ------------------------

//-------- PWM Conf ------------------------

//-------- End Of Defines For Configuration ------

#if (defined VER_1_0) || (defined VER_1_1) || (defined VER_2_0)
//GPIOA pin0	Input_Signal
//GPIOA pin1	I_Sense

//GPIOA pin2	PROT Input
#define PROTECT	((GPIOA->IDR & 0x0004) != 0)

//GPIOA pin3	I_Sense_negado

//GPIOA pin4	NC
//GPIOA pin5	NC

//GPIOA pin6	para TIM3_CH1	LOW_LEFT
//GPIOA pin7	para TIM3_CH2	HIGH_LEFT
//GPIOB pin0	para TIM3_CH3	LOW_RIGHT
//GPIOB pin1	para TIM3_CH4	HIGH_RIGHT

//GPIOA pin8	NC

//GPIOA pin9	usart1 tx
//GPIOA pin10	usart1 rx

//GPIOA pin11	NC

//GPIOA pin12
#define LED ((GPIOA->ODR & 0x1000) != 0)
#define LED_ON	GPIOA->BSRR = 0x00001000
#define LED_OFF GPIOA->BSRR = 0x10000000

//GPIOA pin13	NC
//GPIOA pin14	NC
//GPIOA pin15	NC

//GPIOB pin3	NC
//GPIOB pin4	NC
//GPIOB pin5	NC

//GPIOB pin6
#define STOP_JUMPER ((GPIOB->IDR & 0x0040) == 0)

//GPIOB pin7	NC
#endif	//


//ESTADOS DEL PROGRAMA PRINCIPAL
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

//ESTADOS DEL LED
typedef enum
{    
    START_BLINKING = 0,
    WAIT_TO_OFF,
    WAIT_TO_ON,
    WAIT_NEW_CYCLE
} led_state_t;

//Estados Externos de LED BLINKING
#define LED_NO_BLINKING    0
#define LED_TREATMENT_STANDBY    1
#define LED_TREATMENT_GENERATING    2
#define LED_TREATMENT_JUMPER_PROTECTED    5

//--- Stringtify Utils -----------------------
#define STRING_CONCAT(str1,str2) #str1 " " #str2
#define STRING_CONCAT_NEW_LINE(str1,str2) xstr(str1) #str2 "\n"
#define xstr_macro(s) str_macro(s)
#define str_macro(s) #s

// Module Functions ----------------------------------------------------------
void ChangeLed (unsigned char);
void UpdateLed (void);


#endif /* _HARD_H_ */

//--- end of file ---//

