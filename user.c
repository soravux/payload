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

#include <stdint.h>          /* For uint32_t definition */
#include <stdbool.h>         /* For true/false definition */
#include <stdlib.h>          /* strtok */

#include "user.h"            /* variables/params used by user.c */
#include "system.h"

#include <libpic30.h>        /* __delay_XX() */
#include <uart.h>
#include <string.h>
#include <i2c.h>

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

char GPSData[240];
char* GPSWPtr = GPSData;
char* GPSRPtr = GPSData;
short GPSState = GPS_INVALID;

void initAccelerometer();
void initMagnetometer();
void initGyroscope();

void InitApp(void)
{
    /* Setup analog functionality and port direction */
    //CloseUART1();
    //CloseUART2();
    ANSB = 0x00;

    /* Initialize peripherals */
    /*ConfigIntUART1(UART_RX_INT_EN &
                   UART_RX_INT_PR4 &
                   UART_TX_INT_DIS);*/ /* USE this with GPS */
    ConfigIntUART1(UART_RX_INT_DIS &
                   UART_TX_INT_DIS);
    ConfigIntUART2(UART_RX_INT_DIS &
                   UART_TX_INT_DIS);

    /* Init GPS */
    OpenUART1(UART_EN &
              UART_IDLE_CON &
              UART_DIS_WAKE &
              UART_DIS_LOOPBACK &
              UART_DIS_ABAUD &
              UART_NO_PAR_8BIT &
              UART_1STOPBIT &
              UART_IrDA_DISABLE &
              UART_MODE_SIMPLEX &
              UART_UEN_00 &            //U1TX AND U1RX pins only
              UART_UXRX_IDLE_ONE &
              UART_BRGH_FOUR,
              UART_INT_TX_EACH_CHAR &
              UART_IrDA_POL_INV_ONE &
              UART_SYNC_BREAK_DISABLED &
              UART_TX_DISABLE &
              UART_INT_RX_CHAR &
              UART_ADR_DETECT_DIS &
              UART_RX_OVERRUN_CLEAR,
              FCY / (4 * 38400) - 1);               //BRG=0x22 for 115Kbaud
                                   //BRG=0x1A0 for 9600baud
                                   //BRG=0x010 for 230Kbaud
    /* Init SD Card Writer */
    /*OpenUART2(UART_EN &
              UART_IDLE_STOP &
              UART_DIS_WAKE &
              UART_DIS_LOOPBACK &
              UART_DIS_ABAUD &
              UART_NO_PAR_8BIT &
              UART_1STOPBIT &
              UART_IrDA_DISABLE &
              UART_MODE_SIMPLEX &
              UART_UEN_00 &            //U1TX AND U1RX pins only
              UART_UXRX_IDLE_ONE &
              UART_BRGH_SIXTEEN,
              UART_INT_TX_EACH_CHAR &
              UART_IrDA_POL_INV_ONE &
              UART_SYNC_BREAK_DISABLED &
              UART_TX_ENABLE &
              //UART_INT_RX_CHAR &
              UART_ADR_DETECT_DIS &
              UART_RX_INT_DIS &
              UART_RX_OVERRUN_CLEAR,
              //(FCY / (16 * 9600)) - 1);
              25);*/
    TRISB = 0xFE;

    /* Init I2C peripherals */
    CloseI2C1();
    //DisableIntI2C1();
    OpenI2C1(I2C_MASTER, 0, 39, 0, I2C_MSSP_ENABLE, I2C_SLEW_OFF|I2C_SMBUS_DISABLE);
    /*IdleI2C1();
    StartI2C1();*/
    I2C1_Clear_Intr_Status_Bit;

    /*initAccelerometer();
    initMagnetometer();
    initGyroscope();*/
}

struct GPS_Text processGPS(char* inData)
{
    GPSState = GPS_INVALID;
    /*
     Found GPGGA string.  It has 14 commas total.  Its NMEA sentence structure is:

     $GPGAA,hhmmss.ss,ddmm.mmmm,n,dddmm.mmmm,e,q,ss,y.y,a.a,z,g.g,z,t.t,iii*CC
     |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
     0   	   1         2         3         4         5         6         7
     0123456789012345678901234567890123456789012345678901234567890123456789012

     where:

     GPGAA		: GPS fixed data identifier
     hhmmss.ss	: Coordinated Universal Time (UTC), also known as GMT
     ddmm.mmmm,n	: Latitude in degrees, minutes and cardinal sign
     dddmm.mmmm,e	: Longitude in degrees, minutes and cardinal sign
     q		: Quality of fix.  1 = there is a fix
     ss		: Number of satellites being used
     y.y		: Horizontal dilution of precision
     a.a,M		: GPS antenna altitude in meters
     g.g,M		: geoidal separation in meters
     t.t		: Age of the defferential correction data
     iiii		: Deferential station's ID
     *CC		: checksum for the sentence
  */
    struct GPS_Text data;
    int x = sizeof(struct GPS_Text);
    if (memcmp(inData, "$GPGAA", 6) == 0) {
        memcpy((void*)&data, inData, sizeof(struct GPS_Text));
        /*tmpch = strtok (inData + 6, ",");
        retVal.time = atof(tmpch);
        tmpch = strtok (NULL, ",");
        retVal.latitude = atof(tmpch);
        tmpch = strtok (NULL, ",");
        tmpch = strtok (NULL, ",");
        retVal.longitude = atof(tmpch);*/
    }
    return data;
}

/* OpenLog related.
 * Reference: https://github.com/sparkfun/OpenLog/wiki/Command-Set
 */

void loggerEnterCommandMode()
{
    /* Entering 3x CTRL+Z (26) will enter command mode */
    WriteUART2(26);
    while (BusyUART2());
    WriteUART2(26);
    while (BusyUART2());
    WriteUART2(26);
    while (BusyUART2());
    __delay_ms(5);
    // TODO: check if mode has been changed.
    //WriteUART2(26);
    //while (BusyUART2());
}

void loggerLeaveCommandMode()
{
    /* In Command Mode, CTRL+Z will return in */
    WriteUART2(26);
    while (BusyUART2());
    // TODO: check if mode has been changed.
}

void loggerChangeFile(char *filename, int len)
{
    loggerEnterCommandMode();
    loggerWriteString("append ", 7);
    loggerWriteString(filename, len);
    loggerLeaveCommandMode();
}

void loggerWriteString(char *data, int len)
{
    int i, j;
    /*while (BusyUART2());
    for (i = 0; i < len; ++i) {
        WriteUART2(data[i]);
        while (BusyUART2());
    }*/

    #define BIT __delay_us(102);

    for (j = 0; j < len; ++j) {

        PORTBbits.RB0 = 0;
        BIT

        for (i = 0; i < 8; ++i) {
            PORTBbits.RB0 = 0x01 & (data[j] >> i);
            BIT
        }

        PORTBbits.RB0 = 1;
        BIT
        BIT
    }

}

/* I2C-related code: IMU, Barometer, etc. */
/* This definition is here because in the .h it is ends up being defined multiple times */
char i2c_data[10];


/* IMU-related code. Highly inspired by:
 * https://github.com/ptrbrtz/razor-9dof-ahrs/blob/master/Arduino/Razor_AHRS/Sensors.ino
 */

/* Addr must be in i2c_data[0] and register in i2c_data[1] */
void send_i2c_cmd(char size)
{
    int i;
    StartI2C1(); // Start condition
    IdleI2C1();
    for (i = 0; i <= size; i++) {
        WriteI2C1(i2c_data[i]);
        IdleI2C1();
    }
    StopI2C1(); // Stop condition I2C on bus
}

/* Addr must be in i2c_data[0] and register in i2c_data[1]. Result in i2c_data */
void recv_i2c_cmd(char size)
{
    int i;
    StartI2C1();
    IdleI2C1();
    WriteI2C1(i2c_data[0]); // addresses the chip with a read bit
    IdleI2C1();
    WriteI2C1(i2c_data[1]); // register address
    IdleI2C1();
    RestartI2C1();
    IdleI2C1();
    WriteI2C1(i2c_data[0] + 1);
    IdleI2C1();
    for (i = 0; i <= size; i++) {
        i2c_data[i] = ReadI2C1();
        IdleI2C1();
        if (i < size - 1) {
            AckI2C1();
        }
    }
    NotAckI2C1();
    IdleI2C1();
    StopI2C1(); // Stop condition I2C on bus
}

void initAccelerometer()
{
    i2c_data[0] = ACCEL_ADDRESS;
    /* Power register: Measurement mode */
    memcpy(i2c_data + 1, "\x2D\x08", 2);
    send_i2c_cmd(3);
    __delay_ms(5);
    /* Data format register: Full resolution */
    memcpy(i2c_data + 1, "\x31\x08", 2);
    send_i2c_cmd(3);
    __delay_ms(5);
    /* Rate: 50Hz, normal operation */
    memcpy(i2c_data + 1, "\x2C\x09", 2);
    send_i2c_cmd(3);
    __delay_ms(5);
}

/* Chip model: ADXL345 */
struct axis getAccelerometer()
{
    struct axis ret_val;
    i2c_data[0] = ACCEL_ADDRESS;
    i2c_data[1] = 0x32;

    recv_i2c_cmd(6);

    // LSB first
    ret_val.x = (((int16_t)i2c_data[1]) << 8) | i2c_data[0];
    ret_val.y = (((int16_t)i2c_data[3]) << 8) | i2c_data[2];
    ret_val.z = (((int16_t)i2c_data[5]) << 8) | i2c_data[4];

    return ret_val;
}

void initMagnetometer()
{
    i2c_data[0] = MAGN_ADDRESS;
    /* Set continuous mode (default 10Hz) */
    memcpy(i2c_data + 1, "\x02\x00", 2);
    send_i2c_cmd(3);
    __delay_ms(5);
    /* Rate: 50Hz */
    memcpy(i2c_data + 1, "\x00\x18", 2);
    send_i2c_cmd(3);
    __delay_ms(5);
}

/* Chip model: HMC5883L */
struct axis getMagnetometer()
{
    struct axis ret_val;
    i2c_data[0] = MAGN_ADDRESS;
    i2c_data[1] = 0x03;

    recv_i2c_cmd(6);

    // MSB first
    ret_val.x = (((int16_t)i2c_data[0]) << 8) | i2c_data[1];
    ret_val.y = (((int16_t)i2c_data[2]) << 8) | i2c_data[3];
    ret_val.z = (((int16_t)i2c_data[4]) << 8) | i2c_data[5];

    return ret_val;
}

/* Chip model: ITG3205 */
void initGyroscope()
{
    i2c_data[0] = GYRO_ADDRESS;
    /* Reset chip */
    memcpy(i2c_data + 1, "\x3E\x80", 2);
    send_i2c_cmd(3);
    __delay_ms(5);
    /* Full-range scale with low-pass filter at 42 Hz */
    memcpy(i2c_data + 1, "\x16\x1B", 2);
    send_i2c_cmd(3);
    __delay_ms(5);
    /* Rate: 50 Hz */
    memcpy(i2c_data + 1, "\15\0A", 2);
    send_i2c_cmd(3);
    __delay_ms(5);
    /* Clock to PLL with Z gyro reference */
    memcpy(i2c_data + 1, "\3E\00", 2);
    send_i2c_cmd(3);
    __delay_ms(5);
}

struct axis getGyroscope()
{
    struct axis ret_val;
    i2c_data[0] = GYRO_ADDRESS;
    i2c_data[1] = 0x1D;

    recv_i2c_cmd(6);

    // MSB first
    ret_val.x = (((int16_t)i2c_data[0]) << 8) | i2c_data[1];
    ret_val.y = (((int16_t)i2c_data[2]) << 8) | i2c_data[3];
    ret_val.z = (((int16_t)i2c_data[4]) << 8) | i2c_data[5];

    return ret_val;
}
