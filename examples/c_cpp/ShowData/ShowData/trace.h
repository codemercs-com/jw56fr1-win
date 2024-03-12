
void WriteTrace(CString value);

void TraceString(CString value, CString prefix);
void TraceByte(BYTE value, CString prefix);
void Trace4Byte(BYTE b1, BYTE b2, BYTE b3, BYTE b4, CString prefix);
void TraceInt(int value, CString prefix);
void TraceIntExt(int value, CString prefix, wchar_t delimiter);
void TraceFloat(float value, CString prefix);
void TraceDword(DWORD value, CString prefix);
void TraceByteArray(BYTE value[], int size, CString prefix);

void TraceByteInt(BYTE b, CString prefix);

void TraceHandle(HANDLE value, CString prefix);

