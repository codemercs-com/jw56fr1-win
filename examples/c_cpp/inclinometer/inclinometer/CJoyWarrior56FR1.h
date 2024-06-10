#ifndef CJOYWARRIOR56FR1_H
#define CJOYWARRIOR56FR1_H

#include <windows.h>
#include <iostream>
#include <cstring> //memset

extern "C" {
#include "hidsdi.h" 
#include "setupapi.h"
}

#pragma comment(lib,"hid.lib")
#pragma comment(lib,"setupapi.lib")

//JoyWarrior56FR1 VID and PID
#define CODEMERCS_VID   0x07C0
#define JW56FR1_PID     0x111A

//JoyWarrior56FR1 settings
#define JW56FR1_MAX_DEVICES     16
#define JW56FR1_MAX_INTERFACE   3
#define JW56FR1_MAX_SERIAL      8

//JoyWarrior56FR1 pipe names
#define JW56FR1_PIPE_NORMAL_NAME     L"JW56FR1 Motion Sensor\0"
#define JW56FR1_PIPE_ACC_NAME        L"JW56FR1 High Speed Accel Data\0"
#define JW56FR1_PIPE_ROT_NAME        L"JW56FR1 High Speed Gyro Data\0"

//JoyWarrior56FR1 pipe IDs
#define JW56FR1_PIPE_NORMAL_ID     0
#define JW56FR1_PIPE_ACC_ID        1
#define JW56FR1_PIPE_ROT_ID        2

//JoyWarrior56FR1 report sizes
#define JW56FR1_NORMAL_OUTPUT_SIZE  9       //8 data + ReportID for Windows
#define JW56FR1_NORMAL_INPUT_SIZE   14      //13 data + ReportID for Windows
#define JW56FR1_SPEED_OUTPUT_SIZE   63      //62 data +  ReportID for Windows
#define JW56FR1_SPEED_INPUT_SIZE   63       //62 data +  ReportID for Windows

//JoyWarrior56FR1 mode select
#define JW56FR1_MODE_NORMAL     0x00
#define JW56FR1_MODE_SPEED      0x01

//Conversion values
#define JW56FR1_CONVERSION_ACC_2G			0.061
#define JW56FR1_CONVERSION_ACC_4G			0.122
#define JW56FR1_CONVERSION_ACC_8G			0.244
#define JW56FR1_CONVERSION_ACC_16G			0.488

#define JW56FR1_CONVERSION_GYRO_125			4.375
#define JW56FR1_CONVERSION_GYRO_245			8.75
#define JW56FR1_CONVERSION_GYRO_500			17.50
#define JW56FR1_CONVERSION_GYRO_1000		35.00
#define JW56FR1_CONVERSION_GYRO_2000		70.00

//
#define JW56FR1_2G 16384
#define JW56FR1_4G 8192
#define JW56FR1_8G 4096
#define JW56FR1_16G 2048

//Offset for Joystick center position
#define JW56FR1_ZERO 0x7FFF

//Reports per second
#define JW56FR1_NORMAL_SIZE 833
#define JW56FR1_HIGHSPEED_SIZE 6664

//G value
#define JW56FR1_G_MS 9.81

//Byte ARng
#define JW56FR1_ARNG_RANGE_2G		0x00
#define JW56FR1_ARNG_RANGE_4G		0x02
#define JW56FR1_ARNG_RANGE_8G		0x03
#define JW56FR1_ARNG_RANGE_16G		0x01

//BYTE AFilt
#define JW56FR1_AFILT_NONE			0x00
#define JW56FR1_AFILT_LP1_ONLY		0x00
#define JW56FR1_AFILT_LP1_HP		0x40
#define JW56FR1_AFILT_LP1_LP2		0x80
//#define JW56FR1_AFILT_LP1_HP		0xC0

#define JW56FR1_AFILT_BAND_0		0x00
#define JW56FR1_AFILT_BAND_1		0x20

#define JW56FR1_AFILT_BW_0			0x00	//LP1+LP2
#define JW56FR1_AFILT_BW_1			0x01	//LP1+LP2
#define JW56FR1_AFILT_BW_2			0x02	//LP1+LP2
#define JW56FR1_AFILT_BW_3			0x03	//LP1+LP2
#define JW56FR1_AFILT_BW_4			0x00	//LP1+HP
#define JW56FR1_AFILT_BW_5			0x01	//LP1+HP
#define JW56FR1_AFILT_BW_6			0x02	//LP1+HP
#define JW56FR1_AFILT_BW_7			0x03	//LP1+HP

//Byte GRng
#define JW56FR1_GRNG_245DPS			0x00
#define JW56FR1_GRNG_125DPS			0x01
#define JW56FR1_GRNG_500DPS			0x02
#define JW56FR1_GRNG_1000DPS		0x04
#define JW56FR1_GRNG_2000DPS		0x06

//Byte GFilt
#define JW56FR1_GFILT_NONE			0x00
#define JW56FR1_GFILT_HP_ON			0x80	//HP filter ON
#define JW56FR1_GFILT_HP_OFF		0x00	//HP filter OFF
#define JW56FR1_GFILT_BW_0			0x00	//16mHz
#define JW56FR1_GFILT_BW_1			0x01	//65mHz
#define JW56FR1_GFILT_BW_2			0x02	//260mHz
#define JW56FR1_GFILT_BW_3			0x03	//1.04Hz

//Struct for default joystick interface
struct JW56FR1_DATA
{
    unsigned short accX;
    unsigned short accY;
    unsigned short accZ;
    unsigned short rotX;
    unsigned short rotY;
    unsigned short rotZ;
    unsigned char buttons;
};

struct JW56FR1_DATAEXT
{
    int32_t accX;
    int32_t accY;
    int32_t accZ;
    int32_t rotX;
    int32_t rotY;
    int32_t rotZ;
    unsigned char buttons;
};

struct JW56FR1_DATA_CONVERTED
{
    double accX;
    double accY;
    double accZ;
    double rotX;
    double rotY;
    double rotZ;
    unsigned char buttons;
};

class CJoyWarrior56FR1
{
    private:
    struct JW56FR1_INTERFACE
    {
        HIDP_CAPS caps;
        HANDLE write;
        HANDLE read;
    };

    struct JW56FR1_DEVICE
    {
        char sn[16];
        WCHAR serial[127];
        JW56FR1_INTERFACE iface[JW56FR1_MAX_INTERFACE];
    };

    private:
        unsigned int m_NumDev;           //Number of connected devices
        int m_SelectedDevice;   //Selected device index
        int m_Mode;             //Mode for select interface simple or speed

        double m_ConvAcc;       //Conversion acceleration
        double m_ConvGyro;      //Conversion gyroscope

        JW56FR1_DEVICE m_Device[JW56FR1_MAX_DEVICES];

        void Init();
        
        BOOLEAN GetSerialNumber(HANDLE handle, wchar_t* serial);
        BOOLEAN GetProductString(HANDLE handle, wchar_t* name);
        HIDP_CAPS GetCaps(HANDLE handle);
        
        int Write(unsigned char* data);
        int ReadNormal(unsigned char* data);
        int ReadSpeed(unsigned char id, unsigned char* data);

        void FillSerial(char* serial, char* res);

    protected:

    public:
        CJoyWarrior56FR1();
        virtual ~CJoyWarrior56FR1();

        int Open();                                 //return number of JW56FR1 devices
        void Close();
        int GetNumDev();                            //return number of JW56FR1 devices
        void GetSerial(unsigned int num, char* serial); //return serial number of device (num)
        int Select(unsigned int num);               //Select JW56FR1 by device num
        int Select(char* serial);                   //Select JW56FR1 by serial number
        
        int SetMode(unsigned char mode);            //Set JW56FR1 mode normal or high speed
        int SetParameter(unsigned char ARng, unsigned char AFilt, unsigned char GRng, unsigned char GFilt);  //Set JW56FR1 parameter into RAM
        int WriteParameter();                       //Write parameter into ROM
        
        JW56FR1_DATA GetData();                     //Get normal joystick data as struct
        int GetData(unsigned char* data);           //Get ACC, ROT and Btn data in normal mode as array
        JW56FR1_DATAEXT GetDataZero();              //Get joystick data without offset of joytick interface


        int GetAccSpeed(unsigned char* data);       //Get ACC data in high speed mode as byte array
        int GetRotSpeed(unsigned char* data);       //Get ROT data in high speed mode as byte array

};

#endif // CJOYWARRIOR56FR1_H