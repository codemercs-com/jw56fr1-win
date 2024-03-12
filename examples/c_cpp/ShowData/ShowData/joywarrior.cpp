#include "stdafx.h"
#include "math.h"
#include "joywarrior.h"



CString jw_GetSerialNumber(HANDLE handle)
{
	WCHAR sn[32] = {};
	CString	serial;

	memset(&sn, 0, 32);
	HidD_GetSerialNumberString(handle, &sn, 32);

	serial.Format(_T("%s"), &sn[0]);
	return serial;
}


HIDP_CAPS jw_GetHipdCaps(HANDLE handle)
{
	HIDP_CAPS caps;
	PHIDP_PREPARSED_DATA PreparsedData;

	ZeroMemory(&caps, sizeof(caps));
	ZeroMemory(&PreparsedData, sizeof(PreparsedData));

	HidD_GetPreparsedData(handle, &PreparsedData);
	HidP_GetCaps(PreparsedData, &caps);
	HidD_FreePreparsedData(PreparsedData);

	return caps;
}

int jw_GetDeviceList(ULONG vendorID, ULONG productID, CString devSerial[JW_MAX_DEVICES])
{
	HIDD_ATTRIBUTES	Attributes;
	SP_DEVICE_INTERFACE_DATA devInfoData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA detailData;
	HDEVINFO hDevInfo;
	int MemberIndex = 0;
	LONG DevInfo;
	ULONG Length = 0;
	ULONG Required;
	GUID HidGuid;
	HANDLE devHandle;
	CString serial;
	int	counter = 0;
	int itemCount = 0;

	ZeroMemory(&devInfoData, sizeof(devInfoData));
	devInfoData.cbSize = sizeof(devInfoData);

	HidD_GetHidGuid(&HidGuid);
	hDevInfo = SetupDiGetClassDevsW(&HidGuid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

	do {
		counter = 0;
		DevInfo = SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &HidGuid, MemberIndex, &devInfoData);

		if (DevInfo != 0)
		{
			SetupDiGetDeviceInterfaceDetailW(hDevInfo, &devInfoData, NULL, 0, &Length, NULL);

			detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
			detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			SetupDiGetDeviceInterfaceDetailW(hDevInfo, &devInfoData, detailData, Length, &Required, NULL);

			devHandle = CreateFileW(detailData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, 0);

			Attributes.Size = sizeof(Attributes);
			HidD_GetAttributes(devHandle, &Attributes);

			if ((Attributes.VendorID == vendorID) && (Attributes.ProductID == productID))
			{
				serial.SetString(jw_GetSerialNumber(devHandle));

				for (int i = 0; i < JW_MAX_DEVICES; i++)
				{
					if ((devSerial[i] == serial) || (serial.GetLength() < 1))
						counter++;
				}

				if (counter == 0)
				{
					devSerial[itemCount].SetString(serial);
					itemCount++;
				}
			}

			CloseHandle(devHandle);
			free(detailData);
		}

		MemberIndex++;

	} while (DevInfo != NULL);

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return itemCount;
}

int jw_GetDeviceHandles(ULONG vendorID, ULONG productID, CString serial, JW_HANDLES *handle)
{
	HIDD_ATTRIBUTES	Attributes;
	SP_DEVICE_INTERFACE_DATA devInfoData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA detailData;
	HDEVINFO hDevInfo;
	int	MemberIndex = 0;
	int DeviceIndex = 0;
	LONG DevInfo = 0;
	ULONG Length = 0;
	ULONG Required;
	GUID HidGuid;
	HANDLE dHandle;

	ZeroMemory(&devInfoData, sizeof(devInfoData));
	devInfoData.cbSize = sizeof(devInfoData);

	HidD_GetHidGuid(&HidGuid);

	hDevInfo = SetupDiGetClassDevsW(&HidGuid, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

	do {
		DevInfo = SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &HidGuid, MemberIndex, &devInfoData);

		if (DevInfo != 0)
		{
			SetupDiGetDeviceInterfaceDetailW(hDevInfo, &devInfoData, NULL, 0, &Length, NULL);

			detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
			detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			SetupDiGetDeviceInterfaceDetailW(hDevInfo, &devInfoData, detailData, Length, &Required, NULL);

			dHandle = CreateFileW(detailData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, 0);

			Attributes.Size = sizeof(Attributes);
			HidD_GetAttributes(dHandle, &Attributes);

			if ((Attributes.VendorID == vendorID) && (Attributes.ProductID == productID))
			{
				if ((jw_GetSerialNumber(dHandle) == serial) && (serial.GetLength() > 0))
				{
					//Get HANDLE for read
					handle->read[DeviceIndex] = CreateFileW(detailData->DevicePath,
						GENERIC_WRITE | GENERIC_READ,
						FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,
						FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
						NULL);

					//Get HANDLE for write
					handle->write[DeviceIndex] = CreateFileW(detailData->DevicePath,
						GENERIC_WRITE | GENERIC_READ,
						FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,
						FILE_FLAG_NO_BUFFERING /*| FILE_FLAG_OVERLAPPED*/,
						NULL);

					DeviceIndex++;
				}
			}

			CloseHandle(dHandle);
			free(detailData);
		}

		MemberIndex++;

	} while (DevInfo != NULL);

	SetupDiDestroyDeviceInfoList(hDevInfo);

	handle->count = DeviceIndex;

	return DeviceIndex;
}

int jw_GetData(HANDLE handle, HIDP_CAPS caps, BYTE *data)
{
	OVERLAPPED ovread = { 0 };
	DWORD error = 0;
	DWORD NumberOfBytesRead = 0;
	BOOL Result;
	//HidD_FlushQueue(handle);

	ovread.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

	if (ovread.hEvent == NULL) 
		return E_EVENT;

	Result = ReadFile(handle, data, caps.InputReportByteLength, &NumberOfBytesRead, &ovread);

	if (Result == TRUE)
	{
		error = WaitForSingleObject(ovread.hEvent, 500);

		switch (error)
		{
			case WAIT_OBJECT_0:
			{
				return NumberOfBytesRead;
			}
			case WAIT_TIMEOUT:
			{
				// Timeout error;
				//Cancel the read operation.
				CancelIo(handle);
				NumberOfBytesRead = E_TIMEOUT;
				break;
			}
			default:
			{
				// Undefined error;
				//Cancel the read operation.
				CancelIo(handle);
				NumberOfBytesRead = E_TIMEOUT;
				break;
			}
		}
	}
	
	return NumberOfBytesRead;
}


int jw_SetData(HANDLE handle, HIDP_CAPS caps, BYTE *data)
{
	BOOL Result;
	DWORD BytesWritten = 0;

	Result = WriteFile(handle, data, caps.OutputReportByteLength, &BytesWritten, NULL);

	return BytesWritten;
}

BOOL jw_CloseHandle(HANDLE handle)
{
	return CloseHandle(handle);
}