#include "stdafx.h"
#include <locale.h>
//#include "trace.h"


void WriteTrace(CString value)
{
	value.Trim();
	value.Append(_T("\n"));
	OutputDebugString(value);
}

void TraceString(CString value, CString prefix = NULL)
{
	CString t;
	t.Format(_T("%s %s"), prefix, value);
	WriteTrace(t);
}

void TraceByte(BYTE value, CString prefix = NULL)
{
	CString t;
	t.Format(_T("%s %02X"), prefix, value);
	WriteTrace(t);
}

void Trace4Byte(BYTE b1, BYTE b2, BYTE b3, BYTE b4, CString prefix = NULL)
{
	CString t;
	t.Format(_T("%s %02X %02X %02X %02X"), prefix, b1, b2, b3, b4);
	WriteTrace(t);
}

void TraceInt(int value, CString prefix = NULL)
{
	CString t;
	t.Format(_T("%s%d"), prefix, value);
	WriteTrace(t);
}

void TraceIntExt(int value, CString prefix = NULL, wchar_t delimiter = '.')
{
	CString t;
	CString zahl;

	zahl.Format(_T("%d"), value);

	for (int nIdx = zahl.GetLength() - 3; nIdx > 0; nIdx -= 3)
		zahl.Insert(nIdx, delimiter);

	t.Format(_T("%s %s"), prefix, zahl);
	WriteTrace(t);
}

void TraceFloat(float value, CString prefix = NULL)
{
	CString t;
	t.Format(_T("%s %f"), prefix, value);
	WriteTrace(t);
}

void TraceDword(DWORD value, CString prefix = NULL)
{
	CString t;
	t.Format(_T("%s %04x"), prefix, value);
	WriteTrace(t);
}

void TraceByteArray(BYTE value[], int size, CString prefix = NULL)
{
	CString text, t;

	for (int i = 0; i < size; i++)
	{
		text.Format(_T("%s %02x"), prefix, value[i]);
		t.Append(text);
	}
		
	WriteTrace(t);
}

void TraceByteInt(BYTE b, CString prefix = NULL)
{
	CString t;
	t.Format(_T("%s %02X \t (%d)"), prefix, b, b);
	WriteTrace(t);
}

void TraceHandle(HANDLE value, CString prefix = NULL)
{
	CString t;
	t.Format(_T("%s %x"), prefix, value);
	WriteTrace(t);
}