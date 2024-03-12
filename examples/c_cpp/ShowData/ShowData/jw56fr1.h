#pragma once

#define JW56FR1_ENDPOINT_1 0	//default output
#define JW56FR1_ENDPOINT_2 1	//highspeed ACC
#define JW56FR1_ENDPOINT_3 2	//highspeed GYRO


#define JW56FR1_OFFSET				0x7FFF


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

//Speedmode
#define JW56FR1_SPEEDMODE				0x04
#define JW56FR1_SPEEDMODE_ENABLE		0x01
#define JW56FR1_SPEEDMODE_DISABLE		0x00

//Store para permanent
#define JW56FR1_STORE		0x01

//Misc defines
#define JW56FR1_WRITE_SIZE_DEFAULT 9
#define JW56FR1_READ_SIZE_DEFAULT 14
#define JW56FR1_SPEED_SIZE_SPEED 63


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




typedef struct JW56FR1_DATA
{
	int x;
	int y;
	int z;
	int rx;
	int ry;
	int rz;
	int btn;

} JW56FR1_DATA;


int jw56fr1_SetParams(HANDLE handle, BYTE ARng, BYTE AFilt, BYTE GRng, BYTE GFilt);
int jw56fr1_StoreParams(HANDLE handle);
int jw56fr1_SetMode(HANDLE handle, BYTE state);
int jw56fr1_GetData(HANDLE handle, BYTE *data);
int jw56fr1_GetData(HANDLE handle, JW56FR1_DATA *data);
int jw56fr1_GetDataExt(HANDLE handle, BYTE *data);