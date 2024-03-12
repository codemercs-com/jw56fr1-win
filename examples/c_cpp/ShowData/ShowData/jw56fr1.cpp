#include "stdafx.h"
#include "jw56fr1.h"
#include "joywarrior.h"


int jw56fr1_SetParams(HANDLE handle, BYTE ARng, BYTE AFilt, BYTE GRng, BYTE GFilt)
{
	HIDP_CAPS caps = jw_GetHipdCaps(handle);
	BYTE data[JW56FR1_WRITE_SIZE_DEFAULT] = { 0x00 };

	data[0] = 0x00;		//ReportID
	data[1] = 0x00;		
	data[2] = ARng;
	data[3] = AFilt;
	data[4] = GRng;
	data[5] = GFilt;

	return jw_SetData(handle, caps, data);
}

int jw56fr1_StoreParams(HANDLE handle)
{
	HIDP_CAPS caps = jw_GetHipdCaps(handle);
	BYTE data[JW56FR1_WRITE_SIZE_DEFAULT] = { 0x00 };

	data[0] = 0x00;
	data[1] = 0x01;

	return jw_SetData(handle, caps, data);
}

int jw56fr1_SetMode(HANDLE handle, BYTE state)
{
	HIDP_CAPS caps = jw_GetHipdCaps(handle);
	BYTE data[JW56FR1_WRITE_SIZE_DEFAULT] = { 0x00 };

	data[0] = 0x00;
	data[1] = 0x04;
	data[2] = state;

	return jw_SetData(handle, caps, data);
}

int jw56fr1_GetData(HANDLE handle, BYTE * data)
{
	HIDP_CAPS caps = jw_GetHipdCaps(handle);
	return jw_GetData(handle, caps, data);
}

int jw56fr1_GetData(HANDLE handle, JW56FR1_DATA *data)
{
	int ret = 0;
	BYTE buffer[JW56FR1_READ_SIZE_DEFAULT] = {};
	HIDP_CAPS caps = jw_GetHipdCaps(handle);

	ret =  jw_GetData(handle, caps, buffer);

	data->x = (buffer[1] | (buffer[2] << 8));
	data->y = (buffer[3] | (buffer[4] << 8));
	data->z = (buffer[5] | (buffer[6] << 8));

	data->rx = (buffer[7] | (buffer[8] << 8));
	data->ry = (buffer[9] | (buffer[10] << 8));
	data->rz = (buffer[11] | (buffer[12] << 8));

	data->btn = buffer[13];
	
	return ret;
}

int jw56fr1_GetDataExt(HANDLE handle, BYTE * data)
{
	return 0;
}
