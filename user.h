#ifndef USER_H
#define USER_H
/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/


// Sensor I2C addresses
#define ACCEL_ADDRESS ((int) 0x53) // 0x53 = 0xA6 / 2
#define MAGN_ADDRESS  ((int) 0x1E) // 0x1E = 0x3C / 2
#define GYRO_ADDRESS  ((int) 0x68) // 0x68 = 0xD0 / 2



/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
extern char GPSData[240];
extern char* GPSRPtr;
extern char* GPSWPtr;
extern short GPSState;

typedef enum {
    GPS_INVALID,
    GPS_BEGIN_FOUND,
    GPS_READY,
} GPS_states;

struct GPS_Text {
    char header[6];
    char sep1;
    char utcTime[9];
    char sep2;
    char latitude[11];
    char sep3;
    char longitude[11];
    char sep4;
    char fix_acquired;
    char sep5;
    char num_satellites[2];
    char sep6;
    char horizontalDilutionOfPrecision[3];
    char sep7;
    char altitude[5];
    char sep8;
    char geoidalSeparation[5];
    char sep9;
    char differentialCorrectionDataAge[3];
    char sep10;
    char differentialStationID[4];
    char sep11;
    char checksum[3];
};

struct axis {
    int16_t x;
    int16_t y;
    int16_t z;
};

/* Generic functions */

void InitApp(void);         /* I/O and Peripheral Initialization */


/* Logger functions */

void loggerWriteString(char *data, int len);

/* Sensors functions */

struct GPS_Text processGPS(char* inData);
struct axis getAccelerometer();
struct axis getMagnetometer();
struct axis getGyroscope();

#endif