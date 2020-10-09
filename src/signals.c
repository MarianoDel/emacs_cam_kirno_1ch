//------------------------------------------------
// #### PROYECTO STRETCHER F030 - Power Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### SIGNALS.C ################################
//------------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "signals.h"
#include "hard.h"
#include "stm32f0xx.h"
#include "tim.h"
#include "dsp.h"
#include "dma.h"
#include "adc.h"
#include "usart.h"
#include "gpio.h"

#include <stdio.h>


/* Externals variables ---------------------------------------------------------*/
//del ADC
extern volatile unsigned char seq_ready;
extern volatile unsigned short adc_ch[];

//del Main
extern volatile unsigned short timer_signals;


//de usart para sync
extern volatile unsigned char sync_on_signal;

//del pid dsp.c
extern unsigned short pid_param_p;
extern unsigned short pid_param_i;
extern unsigned short pid_param_d;


/* Global variables ---------------------------------------------------------*/
treatment_t treatment_state = TREATMENT_INIT_FIRST_TIME;
signals_struct_t signal_to_gen;
gen_signal_state_t gen_signal_state = GEN_SIGNAL_INIT;
unsigned char global_error = 0;

unsigned short * p_signal;
unsigned short * p_signal_running;

short d = 0;
short ez1 = 0;
short ez2 = 0;


//-- para determinacion de soft overcurrent ------------
#ifdef USE_SOFT_OVERCURRENT
unsigned short soft_overcurrent_max_current_in_cycles = 0;
unsigned short soft_overcurrent_threshold = 0;
#endif

//-- para determinar no current
//SIZEOF_SIGNALS * max_ADC = 150 * 1023 = 153450
//ojo, depende del salto de indice en la tabla segun la freq elegida
#ifdef USE_SOFT_NO_CURRENT
unsigned int current_integral = 0;
unsigned int current_integral_running = 0;
unsigned char current_integral_ended = 0;
unsigned char current_integral_errors = 0;
#endif

//parametros del PID segun las seniales (es el valor elegido dividido 128)
//PID nuevos
#define PID_SQUARE_P    224
#define PID_SQUARE_I    256
#define PID_SQUARE_D    0

#define PID_TRIANGULAR_P    224
#define PID_TRIANGULAR_I    256
#define PID_TRIANGULAR_D    0

// #define PID_SINUSOIDAL_P    224
#define PID_SINUSOIDAL_P    525
#define PID_SINUSOIDAL_I    256
#define PID_SINUSOIDAL_D    0

//PID original
// #define PID_SQUARE_P    640
// #define PID_SQUARE_I    200
// #define PID_SQUARE_D    0

// #define PID_TRIANGULAR_P    640
// #define PID_TRIANGULAR_I    128
// #define PID_TRIANGULAR_D    0

// #define PID_SINUSOIDAL_P    640
// #define PID_SINUSOIDAL_I    16
// #define PID_SINUSOIDAL_D    0

#ifdef MAX_CURRENT_3_0_A
//Signals Templates
#define I_MAX 465

//seniales nuevas
const unsigned short s_sinusoidal_3A [SIZEOF_SIGNALS] = {14,29,43,58,72,87,101,115,129,143,
                                                         157,171,184,197,211,224,236,249,261,273,
                                                         285,296,307,318,328,338,348,358,367,376,
                                                         384,392,400,407,414,420,426,432,437,442,
                                                         446,450,453,456,459,461,462,464,464,465,
                                                         464,464,462,461,459,456,453,450,446,442,
                                                         437,432,426,420,414,407,400,392,384,376,
                                                         367,358,348,338,328,318,307,296,285,273,
                                                         261,249,236,224,211,197,184,171,157,143,
                                                         129,115,101,87,72,58,43,29,14,0};

const unsigned short s_triangular_3A [SIZEOF_SIGNALS] = {4,9,13,18,23,27,32,37,41,46,
                                                         51,55,60,65,69,74,79,83,88,93,
                                                         97,102,106,111,116,120,125,130,134,139,
                                                         144,148,153,158,162,167,172,176,181,186,
                                                         190,195,199,204,209,213,218,223,227,232,
                                                         237,241,246,251,255,260,265,269,274,279,
                                                         283,288,292,297,302,306,311,316,320,325,
                                                         330,334,339,344,348,353,358,362,367,372,
                                                         376,381,385,390,395,399,404,409,413,418,
                                                         423,427,432,437,441,446,451,455,460,465};


const unsigned short s_square_3A [SIZEOF_SIGNALS] = {465,465,465,465,465,465,465,465,465,465,
                                                     465,465,465,465,465,465,465,465,465,465,
                                                     465,465,465,465,465,465,465,465,465,465,
                                                     465,465,465,465,465,465,465,465,465,465,
                                                     465,465,465,465,465,465,465,465,465,465,
                                                     465,465,465,465,465,465,465,465,465,465,
                                                     465,465,465,465,465,465,465,465,465,465,
                                                     465,465,465,465,465,465,465,465,465,465,
                                                     465,465,465,465,465,465,465,465,465,465,
                                                     465,465,465,465,465,465,465,465,465,465};
#endif

#ifdef MAX_CURRENT_3_6_A
//Signals Templates
#define I_MAX 558

const unsigned short s_triangular_3A [SIZEOF_SIGNALS] = {5,11,16,22,27,33,39,44,50,55,
                                                         61,66,72,78,83,89,94,100,106,111,
                                                         117,122,128,133,139,145,150,156,161,167,
                                                         172,178,184,189,195,200,206,212,217,223,
                                                         228,234,239,245,251,256,262,267,273,279,
                                                         284,290,295,301,306,312,318,323,329,334,
                                                         340,345,351,357,362,368,373,379,385,390,
                                                         396,401,407,412,418,424,429,435,440,446,
                                                         451,457,463,468,474,479,485,491,496,502,
                                                         507,513,518,524,530,535,541,546,552,558};


const unsigned short s_sinusoidal_3A [SIZEOF_SIGNALS] = {17,35,52,69,87,104,121,138,155,172,
                                                         189,205,221,237,253,268,284,298,313,327,
                                                         342,355,369,381,394,406,418,429,440,451,
                                                         461,471,480,488,497,504,512,518,525,530,
                                                         535,540,544,548,551,553,555,556,557,558,
                                                         557,556,555,553,551,548,544,540,535,530,
                                                         525,518,512,504,497,488,480,471,461,451,
                                                         440,429,418,406,394,381,369,355,342,327,
                                                         313,298,284,268,253,237,221,205,189,172,
                                                         155,138,121,104,87,69,52,35,17,0};

const unsigned short s_square_3A [SIZEOF_SIGNALS] = {558,558,558,558,558,558,558,558,558,558,
                                                     558,558,558,558,558,558,558,558,558,558,
                                                     558,558,558,558,558,558,558,558,558,558,
                                                     558,558,558,558,558,558,558,558,558,558,
                                                     558,558,558,558,558,558,558,558,558,558,
                                                     558,558,558,558,558,558,558,558,558,558,
                                                     558,558,558,558,558,558,558,558,558,558,
                                                     558,558,558,558,558,558,558,558,558,558,
                                                     558,558,558,558,558,558,558,558,558,558,
                                                     558,558,558,558,558,558,558,558,558,558};
#endif


// Private Module Functions ------------------------------
void Signal_UpdatePointerReset (void);
resp_t Signal_UpdatePointer (void);
void Signal_DrawingReset (void);
resp_t Signal_Drawing (void);
void Signal_OffsetCalculate (void);

void Signal_Generate_Phase_0_90_120 (void);
void Signal_Generate_Phase_180 (void);
void Signal_Generate_Phase_240 (void);


// Module Functions --------------------------------------
void TreatmentManager (void)
{
    switch (treatment_state)
    {
    case TREATMENT_INIT_FIRST_TIME:
        SIGNAL_PWM_NORMAL_DISCHARGE;
        MA8Circular_Start();

        if (AssertTreatmentParams() == resp_ok)
        {
            treatment_state = TREATMENT_STANDBY;
            ChangeLed(LED_TREATMENT_STANDBY);
        }
        break;

    case TREATMENT_STANDBY:
        break;

    case TREATMENT_START_TO_GENERATE:		//reviso una vez mas los parametros y no tener ningun error
        if ((AssertTreatmentParams() == resp_ok) && (GetErrorStatus() == ERROR_OK))
        {
            Signal_OffsetCalculate();
            GenerateSignalReset();

#ifdef USE_SOFT_OVERCURRENT
            //cargo valor maximo de corriente para el soft_overcurrent
#ifdef USE_SOFT_OVERCURRENT_WITH_PERCENTAGE
            // soft_overcurrent_threshold = 1.2 * I_MAX * signal_to_gen.power / 100;
            // unsigned int over_c = 12 * I_MAX * signal_to_gen.power;
            unsigned int over_c = SIGNAL_ADMITED_WITH_OVERCURRENT * I_MAX * signal_to_gen.power;
            over_c = over_c / 1000;

#endif
#ifdef USE_SOFT_OVERCURRENT_WITH_ABSOLUTE_VALUE
            unsigned int over_c = I_MAX * signal_to_gen.power;
            over_c = over_c / 100;
            over_c += SIGNAL_ADMITED_WITH_OVERCURRENT;
#endif
            soft_overcurrent_threshold = (unsigned short) over_c;
            MA8Circular_Reset();
            soft_overcurrent_max_current_in_cycles = 0;
            
            // char b [64];
            // sprintf(b, "thresh: %d\n", soft_overcurrent_threshold);
            // Usart1Send(b);            
#endif
#ifdef USE_SOFT_NO_CURRENT
            current_integral_errors = 0;
#endif
            EXTIOn();
            treatment_state = TREATMENT_GENERATING;
            ChangeLed(LED_TREATMENT_GENERATING);
        }
        else
        {
            //error de parametros
            treatment_state = TREATMENT_INIT_FIRST_TIME;
        }
        break;

    case TREATMENT_GENERATING:
        //Cosas que dependen de las muestras
        switch (signal_to_gen.offset)
        {
        case ZERO_DEG_OFFSET:
        case NINTY_DEG_OFFSET:
        case HUNDRED_TWENTY_DEG_OFFSET:
            Signal_Generate_Phase_0_90_120();
            break;

        case HUNDRED_EIGHTY_DEG_OFFSET:
            Signal_Generate_Phase_180();
            break;

        case TWO_HUNDRED_FORTY_DEG_OFFSET:
            Signal_Generate_Phase_240();
            break;
            
        default:
            signal_to_gen.offset = ZERO_DEG_OFFSET;
            break;
        }
            

#ifdef USE_SOFT_OVERCURRENT
        if (soft_overcurrent_max_current_in_cycles > soft_overcurrent_threshold)
        {
            soft_overcurrent_max_current_in_cycles = 0;
            treatment_state = TREATMENT_STOPPING;
            SetErrorStatus(ERROR_SOFT_OVERCURRENT);
        }
#endif

#ifdef USE_SOFT_NO_CURRENT
        if (current_integral_ended)
        {
            // char buff [64] = {0};
            // sprintf(buff,"int: %d, err: %d\n",current_integral, current_integral_errors);
            // Usart1Send(buff);
            
            current_integral_ended = 0;
            if (current_integral_errors < CURRENT_INTEGRAL_MAX_ERRORS)
            {
                if (current_integral < CURRENT_INTEGRAL_THRESHOLD)
                {
                    current_integral_errors++;
                    Usart1Send("e\n");
                }
                else if (current_integral_errors)
                    current_integral_errors--;
            }
            else
            {
                treatment_state = TREATMENT_STOPPING;
                SetErrorStatus(ERROR_NO_CURRENT);
            }
        }                
#endif
        break;

    case TREATMENT_STOPPING:
        //espero 3 Tau en descarga normal y voy a idle
        SIGNAL_PWM_NORMAL_DISCHARGE;
        timer_signals = 3 * TAU_LR_MILLIS;
        treatment_state = TREATMENT_STOPPING2;
        break;

    case TREATMENT_STOPPING2:		//aca lo manda directamente la int
        if (!timer_signals)
        {
            treatment_state = TREATMENT_INIT_FIRST_TIME;
            EXTIOff();
            //por si vengo de la INT
            ENABLE_TIM3;
#ifdef LED_SHOW_INT
            LED_OFF;
#endif
        }
        break;

    case TREATMENT_JUMPER_PROTECTED:
        if (!timer_signals)
        {
            if (!STOP_JUMPER)
            {
                treatment_state = TREATMENT_JUMPER_PROTECT_OFF;
                timer_signals = 400;
            }
        }                
        break;

    case TREATMENT_JUMPER_PROTECT_OFF:
        if (!timer_signals)
            treatment_state = TREATMENT_INIT_FIRST_TIME;

        break;
        
    default:
        treatment_state = TREATMENT_INIT_FIRST_TIME;
        break;
    }

    //Cosas que no tienen tanto que ver con las muestras o el estado del programa
    if ((STOP_JUMPER) &&
        (treatment_state != TREATMENT_JUMPER_PROTECTED) &&
        (treatment_state != TREATMENT_JUMPER_PROTECT_OFF))
    {
        SIGNAL_PWM_NORMAL_DISCHARGE;
        timer_signals = 1000;
        treatment_state = TREATMENT_JUMPER_PROTECTED;
        ChangeLed(LED_TREATMENT_JUMPER_PROTECTED);
    }
    
}

// void TreatmentManager_IntSpeed (void)
// {
//     switch (treatment_state)
//     {
//         case TREATMENT_INIT_FIRST_TIME:
//             if (!timer_signals)
//             {
//                 HIGH_LEFT_PWM(0);
//                 LOW_LEFT_PWM(0);
//                 HIGH_RIGHT_PWM(0);
//                 LOW_RIGHT_PWM(DUTY_ALWAYS);

//                 if (GetErrorStatus() == ERROR_OK)
//                 {
//                     GenerateSignalReset();
//                     treatment_state = TREATMENT_GENERATING;
//                     LED_OFF;
//                     EXTIOn();
//                 }
//             }
//             break;

//         case TREATMENT_GENERATING:
//             //Cosas que dependen de las muestras
//             //se la puede llamar las veces que sea necesario y entre funciones, para acelerar
//             //la respuesta
//             GenerateSignal();

//             break;

//         case TREATMENT_STOPPING2:		//aca lo manda directamente la int
//             if (!timer_signals)
//             {
//                 treatment_state = TREATMENT_INIT_FIRST_TIME;
//                 EXTIOff();
//                 ENABLE_TIM3;
// #ifdef LED_SHOW_INT
//                 LED_OFF;
// #endif
//                 SetErrorStatus(ERROR_FLUSH_MASK);
//                 timer_signals = 30;    //30ms mas de demora despues de int
//             }
//             break;

//         default:
//             treatment_state = TREATMENT_INIT_FIRST_TIME;
//             break;
//     }
// }

treatment_t GetTreatmentState (void)
{
    return treatment_state;
}

gen_signal_state_t GetGenSignalState (void)
{
    return gen_signal_state;
}

resp_t StartTreatment (void)
{
    if (treatment_state == TREATMENT_STANDBY)
    {
        if ((AssertTreatmentParams() == resp_ok) && (GetErrorStatus() == ERROR_OK))
        {
            treatment_state = TREATMENT_START_TO_GENERATE;
            return resp_ok;
        }
    }
    return resp_error;
}

void StopTreatment (void)
{
    if (treatment_state != TREATMENT_STANDBY)
        treatment_state = TREATMENT_STOPPING;
}

error_t GetErrorStatus (void)
{
	error_t error = ERROR_OK;

	if (global_error & ERROR_OVERTEMP_MASK)
		error = ERROR_OVERTEMP;
	else if (global_error & ERROR_OVERCURRENT_MASK)
		error = ERROR_OVERCURRENT;
	else if (global_error & ERROR_NO_CURRENT_MASK)
		error = ERROR_NO_CURRENT;
	else if (global_error & ERROR_SOFT_OVERCURRENT_MASK)
		error = ERROR_SOFT_OVERCURRENT;

	return error;
}

void SetErrorStatus (error_t e)
{
    if (e == ERROR_FLUSH_MASK)
        global_error = 0;
    else
    {
        if (e == ERROR_OVERTEMP)
            global_error |= ERROR_OVERTEMP_MASK;
        if (e == ERROR_OVERCURRENT)
            global_error |= ERROR_OVERCURRENT_MASK;
        if (e == ERROR_SOFT_OVERCURRENT)
            global_error |= ERROR_SOFT_OVERCURRENT_MASK;
        if (e == ERROR_NO_CURRENT)
            global_error |= ERROR_NO_CURRENT_MASK;
    }
}

//recibe tipo de senial
//setea senial y offset
resp_t SetSignalTypeAndOffset (signal_type_t a, signal_offset_t offset)
{
    if ((treatment_state != TREATMENT_INIT_FIRST_TIME) && (treatment_state != TREATMENT_STANDBY))
        return resp_error;

    if (offset <= TWO_HUNDRED_FORTY_DEG_OFFSET)
        signal_to_gen.offset = offset;
    else
        return resp_error;

    if (a == SQUARE_SIGNAL)
    {
        p_signal = (unsigned short *) s_square_3A;

        pid_param_p = PID_SQUARE_P;
        pid_param_i = PID_SQUARE_I;
        pid_param_d = PID_SQUARE_D;

        signal_to_gen.signal = a;
    }
#if (defined USE_PROTECTION_WITH_INT) && (defined INT_SPEED_RESPONSE)
    else if (a == TRIANGULAR_SIGNAL)
    {
        p_signal = (unsigned short *) s_triangular_6A;

        pid_param_p = PID_TRIANGULAR_P;
        pid_param_i = PID_TRIANGULAR_I;
        pid_param_d = PID_TRIANGULAR_D;
        
        signal_to_gen.signal = a;
    }
#else
    else if (a == TRIANGULAR_SIGNAL)
    {
        p_signal = (unsigned short *) s_triangular_3A;

        pid_param_p = PID_TRIANGULAR_P;
        pid_param_i = PID_TRIANGULAR_I;
        pid_param_d = PID_TRIANGULAR_D;

        signal_to_gen.signal = a;        
    }
#endif
    else if (a == SINUSOIDAL_SIGNAL)
    {
        p_signal = (unsigned short *) s_sinusoidal_3A;

        pid_param_p = PID_SINUSOIDAL_P;
        pid_param_i = PID_SINUSOIDAL_I;
        pid_param_d = PID_SINUSOIDAL_D;

        signal_to_gen.signal = a;
    }
    else
        return resp_error;
        
    return resp_ok;
}

//recibe referencia a la estructura de senial
//recibe tipo de senial
// resp_t SetSignalType (signals_struct_t * s, signal_type_t a)
// {
//     //TODO: despues cargar directamente los k
//     if ((treatment_state != TREATMENT_INIT_FIRST_TIME) && (treatment_state != TREATMENT_STANDBY))
//         return resp_error;

//     if (a == SQUARE_SIGNAL)
//         p_signal = (unsigned short *) s_cuadrada_1_5A;

// #if (defined USE_PROTECTION_WITH_INT) && (defined INT_SPEED_RESPONSE)
//     if (a == TRIANGULAR_SIGNAL)
//         p_signal = (unsigned short *) s_triangular_6A;
// #else
//     if (a == TRIANGULAR_SIGNAL)
//         p_signal = (unsigned short *) s_triangular_1_5A;    
// #endif

//     if (a == SINUSOIDAL_SIGNAL)
//         p_signal = (unsigned short *) s_senoidal_1_5A;

//     // signal_to_gen.signal = a;
//     s->signal = a;

//     return resp_ok;
// }

//setea la frecuencia y el timer con el que se muestrea
//por default o error es simepre de 1500Hz -> seniales de 10Hz
resp_t SetFrequency (unsigned char entero, unsigned char decimal)
{
    if ((treatment_state != TREATMENT_INIT_FIRST_TIME) && (treatment_state != TREATMENT_STANDBY))
        return resp_error;

    if (decimal >= 100)
        return resp_error;
    
    if ((entero >= FREQ_ALLOWED_MIN) && (entero <= FREQ_ALLOWED_MAX))
    {
        signal_to_gen.freq_int = entero;
        signal_to_gen.freq_dec = decimal;
    }

    return resp_ok;
}

resp_t SetPower (unsigned char a)
{
    if ((treatment_state != TREATMENT_INIT_FIRST_TIME) && (treatment_state != TREATMENT_STANDBY))
        return resp_error;

    if (a > 100)
        signal_to_gen.power = 100;
    else if (a < 10)
        signal_to_gen.power = 10;
    else
        signal_to_gen.power = a;

    return resp_ok;
}

//verifica que se cumplan con todos los parametros para poder enviar una senial coherente
resp_t AssertTreatmentParams (void)
{
    resp_t resp = resp_error;

    if ((signal_to_gen.power > 100) || (signal_to_gen.power < 10))
        return resp;

    //reviso frecuencia a generar
    if (signal_to_gen.freq_dec >= 100)
        return resp;
    
    if ((signal_to_gen.freq_int < FREQ_ALLOWED_MIN) &&
        (signal_to_gen.freq_int > FREQ_ALLOWED_MAX))
        return resp;

    if (signal_to_gen.signal > SINUSOIDAL_SIGNAL)
        return resp;

    if (signal_to_gen.offset > TWO_HUNDRED_FORTY_DEG_OFFSET)
        return resp;
    
    //TODO: revisar tambien puntero  senial!!!!
    return resp_ok;
}

void SendAllConf (void)
{
    char b [64];

    //muestro el canal
    sprintf(b, "channel: %s\n", GetOwnChannel());
    Usart1Send(b);

    //muestro la senial
    switch (signal_to_gen.signal)
    {
    case SQUARE_SIGNAL:
        Usart1Send("signal: SQUARE\n");        
        break;

    case TRIANGULAR_SIGNAL:
        Usart1Send("signal: TRIANGULAR\n");
        break;

    case SINUSOIDAL_SIGNAL:
        Usart1Send("signal: SINUSOIDAL\n");
        break;

    default:
        Usart1Send("signal: error !not loaded!\n");
        break;
    }
    
    //muestro la frecuencia
    sprintf(b, "freq: %d.%02dHz\n",
            signal_to_gen.freq_int,
            signal_to_gen.freq_dec);
    
    Usart1Send(b);

    //muestro el offset
    switch (signal_to_gen.offset)
    {
    case ZERO_DEG_OFFSET:
        Usart1Send("offset: 0deg\n");        
        break;

    case NINTY_DEG_OFFSET:
        Usart1Send("offset: 90deg\n");
        break;

    case HUNDRED_TWENTY_DEG_OFFSET:
        Usart1Send("offset: 120deg\n");
        break;

    case HUNDRED_EIGHTY_DEG_OFFSET:
        Usart1Send("offset: 180deg\n");
        break;

    case TWO_HUNDRED_FORTY_DEG_OFFSET:
        Usart1Send("offset: 240deg\n");
        break;
        
    default:
        Usart1Send("offset: !not loaded!\n");
        break;
    }
    
    //muestro el t1
    sprintf(b, "t1[100us]: %d\n", signal_to_gen.t1);
    Usart1Send(b);

    //muestro la potencia
    sprintf(b, "power: %d\n\n", signal_to_gen.power);
    Usart1Send(b);
}

//reset a antes de la generacion de seniales
void GenerateSignalReset (void)
{
    gen_signal_state = GEN_SIGNAL_INIT;
}

/////////////////////////////////////////////////////////////////
// FUNCIONES SIGNAL_GENERATE_PHASE                             //
// son 3, se encargan de dibujar la senial teniendo en cuenta: //
// * sincronismo                                               //
// * fases seleccionada                                        //
//                                                             //
// Funciones:                                                  //
//  void Signal_Generate_Phase_0_90_120 (void)                  //
//  void Signal_Generate_Phase_180 (void)                      //      
//  void Signal_Generate_Phase_240 (void)                      //
//                                                             //
/////////////////////////////////////////////////////////////////

// Funcion que llama el manager para generar la senial en el canal
// utiliza la senial de synchro desde el puerto serie
// para dibujar la senial llama a Signal_Drawing()
// necesito conocer la fase a generar (hardcoded en la funcion)
// los tiempos de espera, dependen de la frecuencia y de la fase
// el control de soft_overcurrent debiera salir de aca, ya que conozco cuando dibujo o cuando no
// siempre fast discharge hasta que tiene que generar que pasa a normal discharge
// cuando se termine de generar el que llama a esta funcion debera poner normal discharge
void Signal_Generate_Phase_0_90_120 (void)
{

    switch (gen_signal_state)
    {
    case GEN_SIGNAL_INIT:
        SIGNAL_PWM_NORMAL_DISCHARGE;
        gen_signal_state = GEN_SIGNAL_WAIT_FOR_SYNC;

        //sync
        sync_on_signal = 0;

        //no current
#ifdef USE_SOFT_NO_CURRENT
        current_integral_running = 0;
        current_integral_ended = 0;
#endif
        break;

    case GEN_SIGNAL_WAIT_FOR_SYNC:
        if (sync_on_signal)
        {
#ifdef LED_SHOW_SYNC_SIGNAL
            if (LED)
                LED_OFF;
            else
                LED_ON;
#endif

            sync_on_signal = 0;

            TIM16->CNT = 0;
            gen_signal_state = GEN_SIGNAL_WAIT_T1;
        }
        break;

    case GEN_SIGNAL_WAIT_T1:
        if (TIM16->CNT > signal_to_gen.t1)
        {
            SIGNAL_PWM_NORMAL_DISCHARGE;
            sequence_ready_reset;
            
            Signal_DrawingReset ();
            gen_signal_state = GEN_SIGNAL_DRAWING;
        }
        break;
            
    case GEN_SIGNAL_DRAWING:
        //en este bloque tomo la nueva muestra del ADC
        //hago update de la senial antes de cada PID
        //luego calculo el PID y los PWM que correspondan
        if (sequence_ready)
        {
            sequence_ready_reset;    //aprox 7KHz synchro con pwm

#ifdef LED_SHOW_SEQUENCE
            if (LED)
                LED_OFF;
            else
                LED_ON;
#endif
            
            if (Signal_Drawing() == resp_ended) //resuelvo lo referido al final de la senial
                gen_signal_state = GEN_SIGNAL_DRAWING_ENDED;
            else
            {
                //resuelvo lo referido a la senial cuando estoy dibujando
#ifdef USE_SOFT_NO_CURRENT
                current_integral_running += I_Sense;
#endif                

#ifdef USE_SOFT_OVERCURRENT
                soft_overcurrent_max_current_in_cycles = MA8Circular(I_Sense);
#endif
            }
        }
        break;

    case GEN_SIGNAL_DRAWING_ENDED:

        SIGNAL_PWM_FAST_DISCHARGE;
        gen_signal_state = GEN_SIGNAL_WAIT_FOR_SYNC;

#ifdef USE_SOFT_NO_CURRENT
        current_integral = current_integral_running;
        current_integral_running = 0;
        current_integral_ended = 1;
#endif
        break;
            
    case GEN_SIGNAL_STOPPED_BY_INT:		//lo freno la interrupcion
        break;

    default:
        gen_signal_state = GEN_SIGNAL_INIT;
        break;
    }
    
}
    
// Senial especial de 180 grados de defasaje, en la que el synchro
// justo cuando estoy terminando de dibujar la senial o apenas terminado
void Signal_Generate_Phase_180 (void)
{

    switch (gen_signal_state)
    {
    case GEN_SIGNAL_INIT:
        SIGNAL_PWM_NORMAL_DISCHARGE;
        gen_signal_state = GEN_SIGNAL_WAIT_FOR_SYNC;

        //sync
        sync_on_signal = 0;

        //no current
#ifdef USE_SOFT_NO_CURRENT
        current_integral_running = 0;
        current_integral_ended = 0;
#endif
        break;

    case GEN_SIGNAL_WAIT_FOR_SYNC:
        if (sync_on_signal)
        {
#ifdef LED_SHOW_SYNC_SIGNAL
            if (LED)
                LED_OFF;
            else
                LED_ON;
#endif

            sync_on_signal = 0;

            TIM16->CNT = 0;
            gen_signal_state = GEN_SIGNAL_WAIT_T1;
        }
        break;

    case GEN_SIGNAL_WAIT_T1:
        if (TIM16->CNT > signal_to_gen.t1)
        {
            SIGNAL_PWM_NORMAL_DISCHARGE;
            sequence_ready_reset;
            
            Signal_DrawingReset();
            gen_signal_state = GEN_SIGNAL_DRAWING;
        }
        break;
            
    case GEN_SIGNAL_DRAWING:
        //en este bloque tomo la nueva muestra del ADC
        //hago update de la senial antes de cada PID
        //luego calculo el PID y los PWM que correspondan
        if (sequence_ready)
        {
            sequence_ready_reset;    //aprox 7KHz synchro con pwm

#ifdef LED_SHOW_SEQUENCE
            if (LED)
                LED_OFF;
            else
                LED_ON;
#endif
            
            if (Signal_Drawing() == resp_ended)
                gen_signal_state = GEN_SIGNAL_DRAWING_ENDED;
            else
            {
                //resuelvo lo referido a la senial cuando estoy dibujando                
#ifdef USE_SOFT_NO_CURRENT
                current_integral_running += I_Sense;
#endif
                
#ifdef USE_SOFT_OVERCURRENT
                soft_overcurrent_max_current_in_cycles = MA8Circular(I_Sense);
#endif                
            }
        }
        break;

    case GEN_SIGNAL_DRAWING_ENDED:

        SIGNAL_PWM_FAST_DISCHARGE;
        gen_signal_state = GEN_SIGNAL_WAIT_FOR_SYNC;
        
#ifdef USE_SOFT_NO_CURRENT
        current_integral = current_integral_running;
        current_integral_running = 0;
        current_integral_ended = 1;
#endif
        break;
            
    case GEN_SIGNAL_STOPPED_BY_INT:		//lo freno la interrupcion
        break;

    default:
        gen_signal_state = GEN_SIGNAL_INIT;
        break;
    }

    //el synchro en general me llega al final de GEN_SIGNAL_DRAWING
    if ((sync_on_signal) &&
        (gen_signal_state != GEN_SIGNAL_WAIT_FOR_SYNC) &&
        (gen_signal_state == GEN_SIGNAL_DRAWING))
    {
        //no le doy ack al sync
        gen_signal_state = GEN_SIGNAL_DRAWING_ENDED;
    }

}

// Senial especial, defasaje 240 grados, el synchro llega justo cuando estoy dibujando la senial
// de todas formas espero el primer sync para arrancar
// uso sync_on_signal como un doble flag
void Signal_Generate_Phase_240 (void)
{

    switch (gen_signal_state)
    {
    case GEN_SIGNAL_INIT:
        SIGNAL_PWM_NORMAL_DISCHARGE;
        gen_signal_state = GEN_SIGNAL_WAIT_FOR_FIRST_SYNC;

        //sync
        sync_on_signal = 0;

        //no current
#ifdef USE_SOFT_NO_CURRENT
        current_integral_running = 0;
        current_integral_ended = 0;
#endif
        break;
        
    case GEN_SIGNAL_WAIT_FOR_FIRST_SYNC:
        if (sync_on_signal)
        {
            sync_on_signal = 2;
            TIM16->CNT = 0;
            gen_signal_state = GEN_SIGNAL_WAIT_T1;

#ifdef LED_SHOW_SYNC_SIGNAL
            if (LED)
                LED_OFF;
            else
                LED_ON;
#endif            
        }
        break;

    case GEN_SIGNAL_WAIT_T1:
        if ((TIM16->CNT > signal_to_gen.t1) && (sync_on_signal == 2))
        {            
            SIGNAL_PWM_NORMAL_DISCHARGE;
            sequence_ready_reset;
            sync_on_signal = 0;
            
            Signal_DrawingReset ();
            gen_signal_state = GEN_SIGNAL_DRAWING;
        }
        break;
            
    case GEN_SIGNAL_DRAWING:
        //en este bloque tomo la nueva muestra del ADC
        //hago update de la senial antes de cada PID
        //luego calculo el PID y los PWM que correspondan
        if (sequence_ready)
        {
            sequence_ready_reset;    //aprox 7KHz synchro con pwm

#ifdef LED_SHOW_SEQUENCE
            if (LED)
                LED_OFF;
            else
                LED_ON;
#endif
            
            if (Signal_Drawing() == resp_ended)
                gen_signal_state = GEN_SIGNAL_DRAWING_ENDED;
            else
            {
                //resuelvo lo referido a la senial cuando estoy dibujando
#ifdef USE_SOFT_NO_CURRENT
                current_integral_running += I_Sense;
#endif                                

#ifdef USE_SOFT_OVERCURRENT
                soft_overcurrent_max_current_in_cycles = MA8Circular(I_Sense);
#endif
            }
        }
        break;

    case GEN_SIGNAL_DRAWING_ENDED:

        SIGNAL_PWM_FAST_DISCHARGE;
        gen_signal_state = GEN_SIGNAL_WAIT_T1;
        
#ifdef USE_SOFT_NO_CURRENT
        current_integral = current_integral_running;
        current_integral_running = 0;
        current_integral_ended = 1;
#endif
        break;
            
    case GEN_SIGNAL_STOPPED_BY_INT:		//lo freno la interrupcion
        break;

    default:
        gen_signal_state = GEN_SIGNAL_INIT;
        break;
    }

    //el synchro en general me llega en el medio de GEN_SIGNAL_DRAWING
    //uso sync_on_signal como doble flag
    if ((sync_on_signal == 1) && (gen_signal_state != GEN_SIGNAL_WAIT_FOR_FIRST_SYNC))
    {
        sync_on_signal = 2;
        TIM16->CNT = 0;

#ifdef LED_SHOW_SYNC_SIGNAL
            if (LED)
                LED_OFF;
            else
                LED_ON;
#endif        
    }
    
}

typedef enum {
	NORMAL_DISCHARGE = 0,
	TAU_DISCHARGE,
	FAST_DISCHARGE

} drawing_state_t;

drawing_state_t drawing_state = NORMAL_DISCHARGE;

void Signal_DrawingReset (void)
{
    drawing_state = NORMAL_DISCHARGE;
    d = 0;
    ez1 = 0;
    ez2 = 0;

    Signal_UpdatePointerReset();
}

//llamar para cada punto a dibujar
//calculo PID con puntero anterior y actualizo el puntero
resp_t Signal_Drawing (void)
{
    resp_t resp = resp_continue;
    
    switch (drawing_state)
    {
    case NORMAL_DISCHARGE:
        d = PID_roof ((*p_signal_running * signal_to_gen.power / 100),
                      I_Sense,
                      d,
                      &ez1,
                      &ez2);
                    
        //reviso si necesito cambiar a descarga por tau
        if (d < 0)
        {
            HIGH_LEFT_PWM(0);
            drawing_state = TAU_DISCHARGE;
            d = 0;	//limpio para pid descarga
        }
        else
        {
            if (d > DUTY_95_PERCENT)		//no pasar del 95% para dar tiempo a los mosfets
                d = DUTY_95_PERCENT;
            
            HIGH_LEFT_PWM(d);
        }

        if (Signal_UpdatePointer() != resp_continue)
            resp = resp_ended;

        break;

    case TAU_DISCHARGE:		//la medicion de corriente sigue siendo I_Sense
        d = PID_roof ((*p_signal_running * signal_to_gen.power / 100),
                      I_Sense,
                      d,
                      &ez1,
                      &ez2);

        //reviso si necesito cambiar a descarga rapida
        if (d < 0)
        {
            if (-d < DUTY_100_PERCENT)
                LOW_RIGHT_PWM(DUTY_100_PERCENT + d);
            else
                LOW_RIGHT_PWM(0);    //descarga maxima

            drawing_state = FAST_DISCHARGE;
        }
        else
        {
            //esto es normal
            if (d > DUTY_95_PERCENT)		//no pasar del 95% para dar tiempo a los mosfets
                d = DUTY_95_PERCENT;

            HIGH_LEFT_PWM(d);
            drawing_state = NORMAL_DISCHARGE;
        }

        if (Signal_UpdatePointer() != resp_continue)
            resp = resp_ended;

        break;

    case FAST_DISCHARGE:		//la medicion de corriente ahora esta en I_Sense_Neg
        d = PID_roof ((*p_signal_running * signal_to_gen.power / 100),
                      I_Sense_Neg,
                      d,
                      &ez1,
                      &ez2);

        //reviso si necesito cambiar a descarga por tau o normal
        if (d < 0)
        {
            if (-d < DUTY_100_PERCENT)
                LOW_RIGHT_PWM(DUTY_100_PERCENT + d);
            else
                LOW_RIGHT_PWM(0);    //descarga maxima
        }
        else
        {
            //vuelvo a TAU_DISCHARGE
            LOW_RIGHT_PWM(DUTY_ALWAYS);
            drawing_state = TAU_DISCHARGE;
        }

        if (Signal_UpdatePointer() != resp_continue)
            resp = resp_ended;

        break;

    default:
        break;
    }
    return resp;
}

inline void Signal_UpdatePointerReset (void)
{
    p_signal_running = p_signal;
}

resp_t Signal_UpdatePointer (void)
{
    resp_t resp = resp_continue;
    //si la senial esta corriendo hago update de senial y un par de chequeos
    //senial del adc cuando convierte la secuencia disparada por TIM1 a 2000Hz 6000Hz o 12000Hz

    //-- Signal Update --//
    if ((p_signal_running) < (p_signal + SIZEOF_SIGNALS))
    {
        p_signal_running += 1;
    }
    else    //termino la senial, aviso
    {                        
        resp = resp_ended;
    }

    return resp;
}

//calculo el offset de la senial, T1 y el sampling
//el sampling lo seteo en el timer TIM1
//ticks[us] = 1/freq * 1/2 * 1/100 * 1e6 
//TODO: HUNDRED_EIGHTY T1 = T * 180 / 360
void Signal_OffsetCalculate (void)
{
    unsigned int ticks = 1000000;
    unsigned int offset = 1000000;
    unsigned short freq = 0;
    unsigned short freq_2 = 0;
    

    //calculo cantidad de ticks para sampling como 100 puntos en medio ciclo
    //sampling tick 1us
    freq = signal_to_gen.freq_int * 100;
    freq += signal_to_gen.freq_dec;
    freq_2 = freq * 2;
    
    ticks = ticks / freq_2;
    TIM1_ChangeTick((unsigned short) ticks);

    //calculo el t1 en ticks de 100us segun offset
    offset = offset / freq;
    switch (signal_to_gen.offset)
    {
    case ZERO_DEG_OFFSET:
        offset = 0;
        break;

    case NINTY_DEG_OFFSET:
        offset = offset * 90;
        offset = offset / 360;
        break;

    case HUNDRED_TWENTY_DEG_OFFSET:
        offset = offset * 120;
        offset = offset / 360;
        break;

    case HUNDRED_EIGHTY_DEG_OFFSET:
        offset = offset * 180;
        offset = offset / 360;        
        break;

    case TWO_HUNDRED_FORTY_DEG_OFFSET:
        offset = offset * 240;
        offset = offset / 360;        
        break;
    }
    signal_to_gen.t1 = (unsigned short) offset;
    
}

//hubo sobrecorriente, me llaman desde la interrupcion
void Overcurrent_Shutdown (void)
{
#ifdef LED_SHOW_INT
    LED_ON;
#endif

    //primero freno todos los PWM
    HIGH_LEFT_PWM(0);
    LOW_RIGHT_PWM(0);

    DISABLE_TIM3;

    //freno la generacionde la senial
    gen_signal_state = GEN_SIGNAL_STOPPED_BY_INT;

    //ahora aviso del error
    SetErrorStatus(ERROR_OVERCURRENT);

    //meto la generacion en Overcurrent
    timer_signals = 10;
    treatment_state = TREATMENT_STOPPING2;
}

//--- end of file ---//
