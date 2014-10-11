/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */
#if defined(__XC16__)
    #include <xc.h>
#elif defined(__C30__)
    #if defined(__PIC24E__)
    	#include <p24Exxxx.h>
    #elif defined (__PIC24F__)||defined (__PIC24FK__)
	#include <p24Fxxxx.h>
    #elif defined(__PIC24H__)
	#include <p24Hxxxx.h>
    #endif
#endif

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <stdio.h>
#include <uart.h>
#include <time.h>

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp              */

/* This block must be after previous includes, otherwise __delay_XX doesn't work */
#include <libpic30.h>        /* __delay_XX() */
#include <uart.h>
#include <string.h>
#include <i2c.h>

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

/* i.e. uint16_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/


int16_t main(void)
{
    int i;
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize IO ports and peripherals */

    for (i = 0; i < 50; ++i) {
        __delay_ms(100);
    }
    InitApp();

    __delay_ms(100);

    char buffer[31];
    struct axis data;

    while (1)
    {
        /*if (GPSState == GPS_READY) {
            //processGPS(GPSRPtr);
            //loggerWriteString(GPSRPtr, GPSWPtr - GPSRPtr);
            Nop();
        }*/
        
        data = getAccelerometer();
        sprintf(buffer, "%u|Accel: %i, %i, %i\n", (uint16_t)time(NULL), data.x, data.y, data.z);
        loggerWriteString(buffer, 30);
        data = getMagnetometer();
        sprintf(buffer, "%u|Magneto: %i, %i, %i\n", (uint16_t)time(NULL), data.x, data.y, data.z);
        loggerWriteString(buffer, 30);
        data = getGyroscope();
        sprintf(buffer, "%u|Gyro: %i, %i, %i\n", (uint16_t)time(NULL), data.x, data.y, data.z);
        loggerWriteString(buffer, 30);

        __delay_ms(20);
        Nop();
    }
}
