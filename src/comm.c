//------------------------------------------------
// #### PROYECTO STRETCHER F030 - Power Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### COMM.C ###################################
//------------------------------------------------
#include "comm.h"
#include "signals.h"
#include "usart.h"
#include "hard.h"    //para conocer que hacer con el LED, y macros
#include "stm32f0xx.h"

#include "utils.h"

#include <string.h>
#include <stdio.h>


/* Externals variables ---------------------------------------------------------*/
//--- del main
extern const char s_ok [];
//--- del Puerto serie  -------
extern unsigned char usart1_have_data;

/* Global variables ---------------------------------------------------------*/
char buffMessages [100];
const char * p_own_channel;

//strings de comienzo o lineas intermedias
const char s_ch1 [] = {"ch1"};
const char s_ch2 [] = {"ch2"};
const char s_ch3 [] = {"ch3"};
const char s_chf [] = {"chf"};
const char s_set_signal [] = {"signal"};
const char s_frequency [] = {"frequency"};
const char s_power [] = {"power"};

const char s_square [] = {"square"};
const char s_triangular [] = {"triangular"};
const char s_sinusoidal [] = {"sinusoidal"};

const char s_offset_0 [] = {"0"};
const char s_offset_90 [] = {"90"};
const char s_offset_120 [] = {"120"};
const char s_offset_180 [] = {"180"};
const char s_offset_240 [] = {"240"};

const char s_ten_hz [] = {"10Hz"};
const char s_thirty_hz [] = {"30Hz"};
const char s_sixty_hz [] = {"60Hz"};

const char s_start_treatment [] = {"start treatment"};
const char s_stop_treatment [] = {"stop treatment"};
const char s_status [] = {"status"};
const char s_flush_errors [] = {"flush errors"};
const char s_getall [] = {"get all conf"};
const char s_soft_version [] = {"soft version"};

/* Module functions ---------------------------------------------------------*/
void SetOwnChannel (unsigned char ch)
{
    if (ch == 1)
        p_own_channel = s_ch1;
    else if (ch == 2)
        p_own_channel = s_ch2;
    else
        p_own_channel = s_ch3;

}

char * GetOwnChannel (void)
{
    return (char *) p_own_channel;
}

void UpdateCommunications (void)
{
    // si no estoy generando la senial reviso los mensajes
    // if (GetGenSignalState() != GEN_SIGNAL_DRAWING)
    // {
        if (SerialProcess() > 2)	//si tiene algun dato significativo
            InterpretarMsg();
    // }
}

//Procesa consultas desde el micro principal
//carga el buffer buffMessages y avisa con el flag msg_ready
unsigned char SerialProcess (void)
{
    unsigned char bytes_readed = 0;

    if (usart1_have_data)
    {
        usart1_have_data = 0;
        bytes_readed = Usart1ReadBuffer((unsigned char *) buffMessages, sizeof(buffMessages));
    }
    return bytes_readed;
}

resp_t InterpretarMsg (void)
{
    resp_t resp = resp_not_own;
    unsigned char broadcast = 0;
    char * pStr = buffMessages;
    unsigned short new_power = 0;
    unsigned short new_freq_int = 0;
    unsigned short new_freq_dec = 0;
    unsigned char decimales = 0;
    char b [30];

#ifdef LED_SHOW_MSGS
    LED_ON;
#endif
    //reviso canal propio o canal broadcast
    if ((strncmp(pStr, p_own_channel, sizeof(s_chf) - 1) == 0) ||
        (strncmp(pStr, s_chf, sizeof(s_chf) - 1) == 0))
    {
        resp = resp_ok;

        //es broadcast?
        if (*(pStr + 2) == 'f')
            broadcast = 1;

        pStr += sizeof(s_chf);	//normalizo al mensaje, hay un espacio

        //-- Signal Setting
        if (strncmp(pStr, s_set_signal, sizeof(s_set_signal) - 1) == 0)
        {
            pStr += sizeof(s_set_signal);		//normalizo al payload, hay un espacio

            if (strncmp(pStr, s_square, sizeof(s_square) - 1) == 0)
            {
                pStr += sizeof(s_square);		//normalizo al payload, hay un espacio
                
                if (strncmp(pStr, s_offset_0, sizeof(s_offset_0) - 1) == 0)
                    resp = SetSignalTypeAndOffset (SQUARE_SIGNAL, ZERO_DEG_OFFSET);
                else if (strncmp(pStr, s_offset_90, sizeof(s_offset_90) - 1) == 0)
                    resp = SetSignalTypeAndOffset (SQUARE_SIGNAL, NINTY_DEG_OFFSET);
                else if (strncmp(pStr, s_offset_120, sizeof(s_offset_120) - 1) == 0)
                    resp = SetSignalTypeAndOffset (SQUARE_SIGNAL, HUNDRED_TWENTY_DEG_OFFSET);
                else if (strncmp(pStr, s_offset_180, sizeof(s_offset_180) - 1) == 0)
                    resp = SetSignalTypeAndOffset (SQUARE_SIGNAL, HUNDRED_EIGHTY_DEG_OFFSET);
                else if (strncmp(pStr, s_offset_240, sizeof(s_offset_240) - 1) == 0)
                    resp = SetSignalTypeAndOffset (SQUARE_SIGNAL, TWO_HUNDRED_FORTY_DEG_OFFSET);
                else
                    resp = resp_error;
            }
            
            else if (strncmp(pStr, s_triangular, sizeof(s_triangular) - 1) == 0)
            {
                pStr += sizeof(s_triangular);		//normalizo al payload, hay un espacio
                
                if (strncmp(pStr, s_offset_0, sizeof(s_offset_0) - 1) == 0)
                    resp = SetSignalTypeAndOffset (TRIANGULAR_SIGNAL, ZERO_DEG_OFFSET);
                else if (strncmp(pStr, s_offset_90, sizeof(s_offset_90) - 1) == 0)
                    resp = SetSignalTypeAndOffset (TRIANGULAR_SIGNAL, NINTY_DEG_OFFSET);
                else if (strncmp(pStr, s_offset_120, sizeof(s_offset_120) - 1) == 0)
                    resp = SetSignalTypeAndOffset (TRIANGULAR_SIGNAL, HUNDRED_TWENTY_DEG_OFFSET);
                else if (strncmp(pStr, s_offset_180, sizeof(s_offset_180) - 1) == 0)
                    resp = SetSignalTypeAndOffset (TRIANGULAR_SIGNAL, HUNDRED_EIGHTY_DEG_OFFSET);
                else if (strncmp(pStr, s_offset_240, sizeof(s_offset_240) - 1) == 0)
                    resp = SetSignalTypeAndOffset (TRIANGULAR_SIGNAL, TWO_HUNDRED_FORTY_DEG_OFFSET);
                else
                    resp = resp_error;
            }
            
            else if (strncmp(pStr, s_sinusoidal, sizeof(s_sinusoidal) - 1) == 0)
            {
                pStr += sizeof(s_sinusoidal);		//normalizo al payload, hay un espacio
                
                if (strncmp(pStr, s_offset_0, sizeof(s_offset_0) - 1) == 0)
                    resp = SetSignalTypeAndOffset (SINUSOIDAL_SIGNAL, ZERO_DEG_OFFSET);
                else if (strncmp(pStr, s_offset_90, sizeof(s_offset_90) - 1) == 0)
                    resp = SetSignalTypeAndOffset (SINUSOIDAL_SIGNAL, NINTY_DEG_OFFSET);
                else if (strncmp(pStr, s_offset_120, sizeof(s_offset_120) - 1) == 0)
                    resp = SetSignalTypeAndOffset (SINUSOIDAL_SIGNAL, HUNDRED_TWENTY_DEG_OFFSET);
                else if (strncmp(pStr, s_offset_180, sizeof(s_offset_180) - 1) == 0)
                    resp = SetSignalTypeAndOffset (SINUSOIDAL_SIGNAL, HUNDRED_EIGHTY_DEG_OFFSET);
                else if (strncmp(pStr, s_offset_240, sizeof(s_offset_240) - 1) == 0)
                    resp = SetSignalTypeAndOffset (SINUSOIDAL_SIGNAL, TWO_HUNDRED_FORTY_DEG_OFFSET);
                else
                    resp = resp_error;                
            }
            else
                resp = resp_error;
        }

        //-- Frequency Setting
        else if (strncmp(pStr, s_frequency, sizeof(s_frequency) - 1) == 0)
        {
            pStr += sizeof(s_frequency);		//normalizo al payload, hay un espacio

            //lo que viene es E.DD o EE.DD, siempre 2 posiciones decimales
            decimales = StringIsANumber(pStr, &new_freq_int);
            // sprintf(b, "dec: %d, freq_int: %d", decimales, new_freq_int);
            // Usart1Send(b);

            if ((decimales) && (decimales < 3))
            {
                pStr += decimales + 1;    //normalizo con el punto
                decimales = StringIsANumber(pStr, &new_freq_dec);

                // sprintf(b, "dec: %d, freq_int: %d", decimales, new_freq_dec);
                // Usart1Send(b);
                
                if ((decimales > 1) && (decimales < 3))
                    resp = SetFrequency ((unsigned char) new_freq_int, (unsigned char) new_freq_dec);
                else
                    resp = resp_error;
            }
            else
                resp = resp_error;            

        }

        //-- Power Setting
        else if (strncmp(pStr, s_power, sizeof(s_power) - 1) == 0)
        {
            pStr += sizeof(s_power);		//normalizo al payload, hay un espacio

            //lo que viene son 2 o 3 bytes
            decimales = StringIsANumber(pStr, &new_power);
            if ((decimales > 1) && (decimales < 4))
            {
                resp = SetPower (new_power);
                // sprintf(b, "dec: %d, power: %d", decimales, new_power);
                // Usart1Send(b);
            }
            else
                resp = resp_error;
        }

        //-- Start Treatment
        else if (strncmp(pStr, s_start_treatment, sizeof(s_start_treatment) - 1) == 0)
        {
            //se puede empezar
            if (GetTreatmentState() == TREATMENT_STANDBY)
            {
                resp = StartTreatment();
            }
            else
                resp = resp_error;
        }

        //-- Stop Treatment
        else if (strncmp(pStr, s_stop_treatment, sizeof(s_stop_treatment) - 1) == 0)
        {
            StopTreatment();
        }

        //-- Status
        else if (strncmp(pStr, s_status, sizeof(s_status) - 1) == 0)
        {
            switch (GetErrorStatus())
            {
            case ERROR_OK:
                sprintf(b, "%s Manager status: %d\n", p_own_channel, GetTreatmentState());
                Usart1Send(b);
                break;

            case ERROR_OVERCURRENT:
                sprintf(b, "%s Error: Overcurrent\n", p_own_channel);
                Usart1Send(b);
                break;

            case ERROR_NO_CURRENT:
                sprintf(b, "%s Error: No current\n", p_own_channel);
                Usart1Send(b);
                break;

            case ERROR_SOFT_OVERCURRENT:
                sprintf(b, "%s Error: Soft Overcurrent\n", p_own_channel);
                Usart1Send(b);
                break;

            case ERROR_OVERTEMP:
                sprintf(b, "%s Error: Overtemp\n", p_own_channel);
                Usart1Send(b);
                break;

            }
        }

        //-- Soft Version
        else if (strncmp(pStr, s_soft_version, sizeof(s_soft_version) - 1) == 0)
        {
            sprintf(b,"[%s] %s\n", __FILE__, xstr_macro(SOFT_ANNOUNCEMENT));
            Usart1Send(b);
        }

        //-- Flush Errors
        else if (strncmp(pStr, s_flush_errors, sizeof(s_flush_errors) - 1) == 0)
        {
            SetErrorStatus(ERROR_FLUSH_MASK);
        }

        //reviso errores y envio
        // 	error_t e;
        //
        // 	e = GetErrorStatus();
        // 	if (e == ERROR_OK)
        // 	{
        // 		sprintf(b, "Manager status: %d\n", GetTreatmentState());
        // 		Usart1Send(b);
        // 	}
        // 	else
        // 	{
        // 		//tengo algun error, los mando en secuencias
        // 		if (e & ERROR_OVERCURRENT)
        // 			Usart1Send("Error: Overcurrent\n");
        //
        // 		if (e & ERROR_NO_CURRENT)
        // 			Usart1Send("Error: No current\n");
        //
        // 		if (e & ERROR_SOFT_OVERCURRENT)
        // 			Usart1Send("Error: Soft Overcurrent\n");
        //
        // 		if (e & ERROR_OVERTEMP)
        // 			Usart1Send("Error: Overtemp\n");
        // 	}
        // }

        //-- Get All Configuration
        else if (strncmp(pStr, s_getall, sizeof(s_getall) - 1) == 0)
        {
            SendAllConf();
        }

        //-- Ninguno de los anteriores
        else
            resp = resp_error;

    }	//fin if chx

    if (!broadcast)
    {
        if (resp == resp_ok)
            Usart1Send("OK\n");

        if (resp == resp_error)
            Usart1Send("NOK\n");
    }

#ifdef LED_SHOW_MSGS
    LED_OFF;
#endif
    
    return resp;
}

//--- end of file ---//
