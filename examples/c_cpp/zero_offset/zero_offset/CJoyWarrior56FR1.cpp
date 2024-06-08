#include "pch.h"
#include "CJoyWarrior56FR1.h"

CJoyWarrior56FR1::CJoyWarrior56FR1()
{
    Init();
}

CJoyWarrior56FR1::~CJoyWarrior56FR1()
{
    
}

void CJoyWarrior56FR1::Init()
{
    m_NumDev = 0;

    for (int i = 0; i < JW56FR1_MAX_DEVICES; i++)
	{
        for(int j=0; j<JW56FR1_MAX_INTERFACE; j++)
        {
			m_Device[i].iface[j].write = INVALID_HANDLE_VALUE;
			m_Device[i].iface[j].read = INVALID_HANDLE_VALUE;
        }
	}
}

BOOLEAN CJoyWarrior56FR1::GetSerialNumber(HANDLE handle, wchar_t* serial)
{
	wchar_t sn[32] = {'\0'};
	BOOLEAN ret = HidD_GetSerialNumberString(handle, sn, (sizeof(wchar_t) * JW56FR1_MAX_SERIAL));
	return ret;
}

BOOLEAN CJoyWarrior56FR1::GetProductString(HANDLE handle, wchar_t* name)
{
	wchar_t buffer[255] = {};
	memset(&buffer, 0, 255);
	BOOLEAN ret = HidD_GetProductString(handle, &buffer, 255);
	memcpy(name, buffer, sizeof(buffer));

	return ret;
}

HIDP_CAPS CJoyWarrior56FR1::GetCaps(HANDLE handle)
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

int CJoyWarrior56FR1::Open()
{
    HIDD_ATTRIBUTES	Attributes;
	SP_DEVICE_INTERFACE_DATA devInfoData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA detailData;
	HDEVINFO hDevInfo;
	int	MemberIndex = 0;
	int DeviceIndex = 0;
	long DevInfo = 0;
	unsigned long Length = 0;
	unsigned long Required;
	GUID HidGuid;
	HANDLE dHandle;

	//List of serials
	BOOLEAN ret = FALSE;
	char serials[JW56FR1_MAX_DEVICES][16];
	int devCount = 0;
	int itemCount = -1;

	char c_serial[16] = {'\0'};
	char c_name[255] = {'\0'};

	WCHAR serial_buffer[127] = {'\0'};
	wchar_t name_buffer[512] = {'\0'};


    Init();
	m_NumDev = 0;
	int pipe_num = 0;

	ZeroMemory(&devInfoData, sizeof(devInfoData));
	devInfoData.cbSize = sizeof(devInfoData);

	HidD_GetHidGuid(&HidGuid);

	hDevInfo = SetupDiGetClassDevs(&HidGuid, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

	do {
		devCount = 0;
		DevInfo = SetupDiEnumDeviceInterfaces(hDevInfo, 0, &HidGuid, MemberIndex, &devInfoData);

		if (DevInfo != 0)
		{
			SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, 0, 0, &Length, 0);

			detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
			detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			BOOL result  = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, detailData, Length, &Required, 0);

			dHandle = INVALID_HANDLE_VALUE;
			dHandle = CreateFile(detailData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, 0);
			
			Attributes.Size = sizeof(Attributes);
			HidD_GetAttributes(dHandle, &Attributes);

			if (dHandle != INVALID_HANDLE_VALUE)
			{
				if ((Attributes.VendorID == CODEMERCS_VID) && (Attributes.ProductID == JW56FR1_PID))
				{
					ret = HidD_GetSerialNumberString(dHandle, serial_buffer, sizeof(serial_buffer));
					sprintf(c_serial, "%ls", serial_buffer);

					ret = HidD_GetProductString(dHandle, &name_buffer, 512);

					if(wcscmp(name_buffer, JW56FR1_PIPE_NORMAL_NAME) == 0) pipe_num = 0;
					if(wcscmp(name_buffer, JW56FR1_PIPE_ACC_NAME) == 0) pipe_num = 1;
					if(wcscmp(name_buffer, JW56FR1_PIPE_ROT_NAME) == 0) pipe_num = 2;

					//Check for new serials / new JW56FR1
					for (int i = 0; i < JW56FR1_MAX_DEVICES; i++)
					{
						if((strcmp(serials[i], c_serial) == 0) || (strlen(c_serial) < 1))
							devCount++;
					}

					if (devCount == 0)
					{
						m_NumDev++;
						itemCount++;

						strcpy(serials[itemCount], c_serial);		//Copy serial into list for unique devices
						strcpy(m_Device[itemCount].sn, c_serial);	//Copy serial into struct
					}

					m_Device[itemCount].iface[pipe_num].write = CreateFile(detailData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, 0);
					m_Device[itemCount].iface[pipe_num].read = CreateFile(detailData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, 0);
					m_Device[itemCount].iface[pipe_num].caps = GetCaps(m_Device[itemCount].iface[pipe_num].write);

					DeviceIndex++;
				}
			}

			CloseHandle(dHandle);
			free(detailData);
		}
        
		MemberIndex++;

	} while (DevInfo != NULL);

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return m_NumDev;
}

void CJoyWarrior56FR1::Close()
{
	CloseHandle(m_Device[0].iface[0].write);
}

//Write only possible to interface 0
int CJoyWarrior56FR1::Write(unsigned char* data)
{
	BOOL Result = FALSE;
	DWORD BytesWritten = 0;

	if(m_SelectedDevice > -1)
	{
		Result = WriteFile(m_Device[m_SelectedDevice].iface[JW56FR1_PIPE_NORMAL_ID].write, data, JW56FR1_NORMAL_OUTPUT_SIZE, &BytesWritten, NULL);

		if(Result != TRUE)
			return 0;
	}
	else
		std::cout << __FUNCTION__ << "() " << "No device" << std::endl;

	return BytesWritten;
}

int CJoyWarrior56FR1::ReadNormal(unsigned char* data)
{
	DWORD BytesRead = 0;
	DWORD bytes = 0;
	DWORD ret = 0;
	ULONG bResult = FALSE;
	ULONG dwError = 0;
	unsigned char buffer[JW56FR1_NORMAL_INPUT_SIZE] = {0};

	OVERLAPPED o = { 0 };
	o.hEvent = CreateEvent(NULL, NULL, NULL, NULL);

	if (o.hEvent == NULL)
	{
		std::cout << "CreateEvent() not created" << std::endl;
		return 0;
	}
	
	bResult = ReadFile(m_Device[m_SelectedDevice].iface[JW56FR1_PIPE_NORMAL_ID].read, buffer, (DWORD)JW56FR1_NORMAL_INPUT_SIZE, (LPDWORD)&BytesRead, &o);
	if(bResult == TRUE)
	{
		//dwError = GetLastError();
		DWORD dwWaitRes = WaitForSingleObject(o.hEvent, 1000);

		switch (dwWaitRes)
		{
		case WAIT_OBJECT_0:
			//std::cout << __FUNCTION__ << "(): WAIT_OBJECT_0" << std::endl;
			bResult = GetOverlappedResult(m_Device[m_SelectedDevice].iface[JW56FR1_PIPE_NORMAL_ID].read, &o, (LPDWORD)&bytes, FALSE);
			ret = bytes;
			CloseHandle(o.hEvent);

			break;
		case WAIT_TIMEOUT:
			//std::cout << __FUNCTION__ << "(): WAIT_TIMEOUT" << std::endl;
			ret = 0;
			CancelIo(m_Device[m_SelectedDevice].iface[JW56FR1_PIPE_NORMAL_ID].read);
			CloseHandle(o.hEvent);
			break;
		}

		memcpy(data, buffer, JW56FR1_NORMAL_INPUT_SIZE);
	}
	return ret;
}

int CJoyWarrior56FR1::ReadSpeed(unsigned char id, unsigned char* data)
{
DWORD BytesRead = 0;
	DWORD bytes = 0;
	DWORD ret = 0;
	ULONG bResult = FALSE;
	ULONG dwError = 0;
	unsigned char buffer[JW56FR1_SPEED_INPUT_SIZE] = {0};

	OVERLAPPED o = { 0 };
	o.hEvent = CreateEvent(NULL, NULL, NULL, NULL);

	if (o.hEvent == NULL)
		std::cout << "CreateEvent() not created" << std::endl;
	
	bResult = ReadFile(m_Device[m_SelectedDevice].iface[id].read, buffer, (DWORD)JW56FR1_SPEED_INPUT_SIZE, (LPDWORD)&BytesRead, &o);
	if(bResult == TRUE)
	{
		//dwError = GetLastError();
		DWORD dwWaitRes = WaitForSingleObject(o.hEvent, 1000);

		switch (dwWaitRes)
		{
		case WAIT_OBJECT_0:
			//std::cout << __FUNCTION__ << "(): WAIT_OBJECT_0" << std::endl;
			bResult = GetOverlappedResult(m_Device[m_SelectedDevice].iface[id].read, &o, (LPDWORD)&bytes, FALSE);
			ret = bytes;
			CloseHandle(o.hEvent);

			break;
		case WAIT_TIMEOUT:
			//std::cout << __FUNCTION__ << "(): WAIT_TIMEOUT" << std::endl;
			ret = 0;
			CancelIo(m_Device[m_SelectedDevice].iface[id].read);
			CloseHandle(o.hEvent);
			break;
		}

		memcpy(data, buffer, JW56FR1_SPEED_INPUT_SIZE);
	}
	return ret;
}

int CJoyWarrior56FR1::GetNumDev()
{
	return m_NumDev;
}

void CJoyWarrior56FR1::GetSerial(unsigned int num, char* serial)
{
	if(m_NumDev > 0)
		strcpy(serial, m_Device[num].sn);
	else
		std::cout << "ERROR: " << __FUNCTION__ << std::endl;
}

void CJoyWarrior56FR1::FillSerial(char* serial, char* res)
{
	char sn[16] = {"0"};
	char tmp[16] = {"00000000"};

	strcpy(sn, serial);
	strcpy(tmp + (strlen(tmp)-strlen(sn)), serial);
	strcpy(res, tmp);
}

int CJoyWarrior56FR1::Select(unsigned int num)
{
	if(num > m_NumDev)
	{
		m_SelectedDevice = 0;
		return 0;
	}
	else
		m_SelectedDevice = num;

	return 1;
}

int CJoyWarrior56FR1::Select(char* serial)
{
	char tmp[16] = {"\0"};
	m_SelectedDevice = -1;

	if(strlen(serial) < 8)
		FillSerial(serial, tmp);
	else
		strcpy(tmp, serial);

	for(unsigned int i=0; i < m_NumDev; i++)
	{
		if(strcmp(m_Device[i].sn, tmp) == 0)
		{
			m_SelectedDevice = i;
			break;
		}
	}

	if(m_SelectedDevice == -1)
	{
		std::cout << "No device selected, wrong serial or more than 8 chars" << std::endl;
		return 0;
	}

	return 1;
}

int CJoyWarrior56FR1::SetMode(unsigned char mode)
{
	unsigned char data[JW56FR1_NORMAL_OUTPUT_SIZE] = { 0x00, 0x04, (unsigned char)(mode & 0x01) };
	m_Mode = (mode & 0x01);
	return Write(data);
}

int CJoyWarrior56FR1::SetParameter(unsigned char ARng, unsigned char AFilt, unsigned char GRng, unsigned char GFilt)
{
	unsigned char data[JW56FR1_NORMAL_OUTPUT_SIZE] = { 0x00, 0x00, ARng, AFilt, GRng, GFilt};

	/*
	switch (ARng)
	{
	case JW56FR1_ARNG_RANGE_2G: m_ConvAcc = JW56FR1_CONVERSION_ACC_2G;
	case JW56FR1_ARNG_RANGE_4G: m_ConvAcc = JW56FR1_CONVERSION_ACC_4G;
	case JW56FR1_ARNG_RANGE_8G: m_ConvAcc = JW56FR1_CONVERSION_ACC_8G;
	case JW56FR1_ARNG_RANGE_16G: m_ConvAcc = JW56FR1_CONVERSION_ACC_16G;
	default: m_ConvAcc = JW56FR1_CONVERSION_ACC_2G;
	}

	switch (GRng)
	{
	case JW56FR1_GRNG_245DPS: m_ConvGyro = JW56FR1_CONVERSION_GYRO_245;
	case JW56FR1_GRNG_125DPS: m_ConvGyro = JW56FR1_CONVERSION_GYRO_125;
	case JW56FR1_GRNG_500DPS: m_ConvGyro = JW56FR1_CONVERSION_GYRO_500;
	case JW56FR1_GRNG_1000DPS: m_ConvGyro = JW56FR1_CONVERSION_GYRO_1000;
	case JW56FR1_GRNG_2000DPS: m_ConvGyro = JW56FR1_CONVERSION_GYRO_2000;
	default: m_ConvGyro = JW56FR1_CONVERSION_GYRO_245;
	}
	*/

	return Write(data);
}

int CJoyWarrior56FR1::WriteParameter()
{
	unsigned char data[JW56FR1_NORMAL_OUTPUT_SIZE] = { 0x00, 0x01 };
	return Write(data);
}

JW56FR1_DATA CJoyWarrior56FR1::GetData()
{
	unsigned char buffer[JW56FR1_NORMAL_INPUT_SIZE] = {0};
	JW56FR1_DATA joy = { 0 };

	if(ReadNormal(buffer) == JW56FR1_NORMAL_INPUT_SIZE)
	{
		joy.accX = (buffer[1] | (buffer[2] << 8));
		joy.accY = (buffer[3] | (buffer[4] << 8));
		joy.accZ = (buffer[5] | (buffer[6] << 8));

		joy.rotX = (buffer[7] | (buffer[8] << 8));
		joy.rotY = (buffer[9] | (buffer[10] << 8));
		joy.rotZ = (buffer[11] | (buffer[12] << 8));

		joy.buttons = buffer[13];
	}
	else
		std::cout << __FUNCTION__ << "() Error on read" << std::endl;

	return joy;
}

int CJoyWarrior56FR1::GetData(unsigned char* data)
{
	unsigned char buffer[JW56FR1_NORMAL_INPUT_SIZE] = {0};
	int ret = ReadNormal(buffer);
	memcpy(data, buffer, JW56FR1_NORMAL_INPUT_SIZE);
	return ret;
}

JW56FR1_DATAEXT CJoyWarrior56FR1::GetDataZero()
{
	unsigned char buffer[JW56FR1_NORMAL_INPUT_SIZE] = { 0 };
	JW56FR1_DATAEXT joy = { 0 };

	if (ReadNormal(buffer) == JW56FR1_NORMAL_INPUT_SIZE)
	{
		joy.accX = (buffer[1] | (buffer[2] << 8)) - JW56FR1_ZERO;
		joy.accY = (buffer[3] | (buffer[4] << 8)) - JW56FR1_ZERO;
		joy.accZ = (buffer[5] | (buffer[6] << 8)) - JW56FR1_ZERO;

		joy.rotX = (buffer[7] | (buffer[8] << 8)) - JW56FR1_ZERO;
		joy.rotY = (buffer[9] | (buffer[10] << 8)) - JW56FR1_ZERO;
		joy.rotZ = (buffer[11] | (buffer[12] << 8)) - JW56FR1_ZERO;

		joy.buttons = buffer[13];
	}
	else
		std::cout << __FUNCTION__ << "() Error on read" << std::endl;

	return joy;
}

/*
JW56FR1_DATA_CONVERTED CJoyWarrior56FR1::GetDataExt()
{
	unsigned char buffer[JW56FR1_NORMAL_INPUT_SIZE] = { 0 };
	JW56FR1_DATA_CONVERTED joy;

	if (ReadNormal(buffer) == JW56FR1_NORMAL_INPUT_SIZE)
	{
		joy.accX = ((buffer[1] | (buffer[2] << 8)) - 0x7FFF) * m_ConvAcc;
		joy.accY = ((buffer[3] | (buffer[4] << 8)) - 0x7FFF) * m_ConvAcc;
		joy.accZ = ((buffer[5] | (buffer[6] << 8)) - 0x7FFF) * m_ConvAcc;

		joy.rotX = ((buffer[7] | (buffer[8] << 8)) - 0x7FFF) * m_ConvGyro;
		joy.rotY = ((buffer[9] | (buffer[10] << 8)) - 0x7FFF) * m_ConvGyro;
		joy.rotZ = ((buffer[11] | (buffer[12] << 8)) - 0x7FFF) * m_ConvGyro;

		joy.buttons = buffer[13];
	}
	else
		std::cout << __FUNCTION__ << "() Error on read" << std::endl;

	return joy;
}
*/

int CJoyWarrior56FR1::GetAccSpeed(unsigned char* data)
{
	unsigned char buffer[JW56FR1_SPEED_INPUT_SIZE] = {0};
	int ret = ReadSpeed(JW56FR1_PIPE_ACC_ID, buffer);
	memcpy(data, buffer, JW56FR1_SPEED_INPUT_SIZE);

	return ret;
}
int CJoyWarrior56FR1::GetRotSpeed(unsigned char* data)
{
	unsigned char buffer[JW56FR1_SPEED_INPUT_SIZE] = {0};
	int ret = ReadSpeed(JW56FR1_PIPE_ROT_ID, buffer);
	memcpy(data, buffer, JW56FR1_SPEED_INPUT_SIZE);

	return ret;
}