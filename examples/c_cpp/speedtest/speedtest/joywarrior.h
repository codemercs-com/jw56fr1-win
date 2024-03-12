/*HID functions for JoyWarrior family*/
#pragma once 

extern "C" {
#include "hidsdi.h" 
#include "setupapi.h"
}

#pragma comment(lib,"hid.lib")
#pragma comment(lib,"setupapi.lib")

//VendorID from Codemercs
#define CODEMERCS_VID 0x07C0

//ProductIDs for USB-Devices from Codemercs
#define JW24F8_PID 0x1113
#define JW24F14_PID 0x1116
#define JW56FR1_PID 0x111A

//misc defines
#define JW_MAX_DEVICES 8

//JoyWarrior24F8
#define JW24F8_ENDPOINTS 2

//JoyWarrior24F14
#define JW24F14_ENDPOINTS 2

//JoyWarrior56FR1
#define JW56FR1_ENDPOINTS 3



//Errorcodes
#define E_SUCCESS	0	
#define E_READ		-1
#define E_TIMEOUT	-2
#define E_EVENT		-4

//vars
typedef struct JW_DEVICE
{
	int devCount;
	HANDLE handles[8];
	CString serial;
} HID_DEVICES;


typedef struct JW_HANDLES
{
	int count;
	HANDLE write[4];
	HANDLE read[4];

} JW_HANDLES;



//functions
CString jw_GetSerialNumber(HANDLE handle);
HIDP_CAPS jw_GetHipdCaps(HANDLE handle);

int jw_GetDeviceList(ULONG vendorID, ULONG productID, CString devSerial[JW_MAX_DEVICES]);
//int jw_GetDeviceHandles(ULONG vendorID, ULONG productID, CString serial, HANDLE *handle);
int jw_GetDeviceHandles(ULONG vendorID, ULONG productID, CString serial, JW_HANDLES *handle);

int jw_GetData(HANDLE handle, HIDP_CAPS caps, BYTE *data);
int jw_SetData(HANDLE handle, HIDP_CAPS caps, BYTE *data);

BOOL jw_CloseHandle(HANDLE handle);
