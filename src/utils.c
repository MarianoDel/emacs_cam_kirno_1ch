//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### UTILS.C ###############################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "utils.h"

#include <stdlib.h>

//--- VARIABLES EXTERNAS ---//

//--- VARIABLES GLOBALES ---//


//--- FUNCIONES DEL MODULO ---//
//devuelve los numeros de un string, en la posicion number
//devuele la cantidad de cifras leidas
unsigned char StringIsANumber (char * pn, unsigned short * number)
{
    unsigned char i;
    char new_number [6] = {0};

    //no mas de 6 caracteres
    for (i = 0; i < 6; i++)
    {
        if ((*(pn + i) < '0') || (*(pn + i) > '9'))
            break;

        new_number[i] = *(pn + i);
    }

    if (i > 0)
        *number = atoi(new_number);

    return i;
}


unsigned short GetValue (unsigned char * pn, char delimiter)
{
	unsigned char i;
	unsigned char colon = 0;
	unsigned short new_val = 0xffff;

	//me fijo la posiciones de la , o ;
	for (i = 0; i < 6; i++)
	{
		if (*(pn + i) == delimiter)
		{
			colon = i;
			i = 6;
		}
	}

	if ((colon == 0) || (colon >= 5))
		return 0;

	switch (colon)
	{
		case 1:
			new_val = *pn - '0';
			break;

		case 2:
			new_val = (*pn - '0') * 10 + (*(pn + 1) - '0');
			break;

		case 3:
			new_val = (*pn - '0') * 100 + (*(pn + 1) - '0') * 10 + (*(pn + 2) - '0');
			break;

		case 4:
			new_val = (*pn - '0') * 1000 + (*(pn + 1) - '0') * 100 + (*(pn + 2) - '0') * 10 + (*(pn + 2) - '0');
			break;

	}
	return new_val;
}

//--- end of file ---//
