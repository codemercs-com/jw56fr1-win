
// speedtestDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "speedtest.h"
#include "speedtestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CspeedtestDlg-Dialogfeld



CspeedtestDlg::CspeedtestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SPEEDTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_ModeType = 0;
}

void CspeedtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_DeviceList);
	DDX_Control(pDX, IDC_CMB_MODE, m_Mode);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Control(pDX, IDC_COMBO2, m_ARange);
	DDX_Control(pDX, IDC_COMBO4, m_AFilter);
	DDX_Control(pDX, IDC_COMBO5, m_ABand);
	DDX_Control(pDX, IDC_COMBO6, m_ACut);
	DDX_Control(pDX, IDC_COMBO7, m_RCut);
	DDX_Control(pDX, IDC_COMBO3, m_RRange);
	DDX_Control(pDX, IDC_CHECK1, m_RHp);
}

BEGIN_MESSAGE_MAP(CspeedtestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &CspeedtestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CspeedtestDlg::OnBnClickedButton4)
	ON_CBN_SELCHANGE(IDC_CMB_MODE, &CspeedtestDlg::OnCbnSelchangeCmbMode)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CspeedtestDlg::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO4, &CspeedtestDlg::OnCbnSelchangeCombo4)
END_MESSAGE_MAP()


// CspeedtestDlg-Meldungshandler

BOOL CspeedtestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Symbol für dieses Dialogfeld festlegen.  Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	m_bRun = false;
	m_ModeType = JW56FR1_ENDPOINT_ACC_SPEED;
	m_Mode.SetCurSel(0);
	m_Progress.SetRange(0, 10);
	m_Progress.SetStep(1);
	m_ProgCount = 0;

	m_ARange.SetCurSel(0);
	m_AFilter.SetCurSel(0);
	m_ABand.SetCurSel(0);
	m_ACut.SetCurSel(0);
	m_RRange.SetCurSel(0);
	m_RCut.SetCurSel(0);

	m_ACut.EnableWindow(FALSE);

	m_Mode.SetCurSel(0);

	CString text;
	text.Format(L"Version: %s", GetVersion());
	GetDlgItem(IDC_STATIC_VERSION)->SetWindowTextW(text);


	return TRUE;  // TRUE zurückgeben, wenn der Fokus nicht auf ein Steuerelement gesetzt wird
}

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie
//  den nachstehenden Code, um das Symbol zu zeichnen.  Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

void CspeedtestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext zum Zeichnen

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Symbol in Clientrechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CspeedtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CALLBACK CspeedtestDlg::MM_TimerFunction(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	CspeedtestDlg* obj = (CspeedtestDlg*)dwUser;
	obj->MM_TimerHandler(wTimerID);
}

UINT CspeedtestDlg::MM_SetTimer(UINT ms)
{
	UINT eventID = 0;
	TIMECAPS tc;

	timeGetDevCaps(&tc, sizeof(TIMECAPS));

	DWORD resolution = min(max(tc.wPeriodMin, 0), tc.wPeriodMax);
	timeBeginPeriod(resolution);

	eventID = timeSetEvent(ms, resolution, MM_TimerFunction, (DWORD)this, TIME_PERIODIC);
	return eventID;
}

void CspeedtestDlg::MM_KillTimer(UINT nIDEvent)
{
	timeKillEvent(nIDEvent);
}

void CspeedtestDlg::MM_TimerHandler(UINT nIDEvent)
{
	CString csv;
	BYTE buffer[JW56FR1_SPEED_SIZE_SPEED] = { 0 };
	CString values;


	//HIDP_CAPS caps = jw_GetHipdCaps(m_Device.read[JW56FR1_ENDPOINT_ROT_SPEED]);
	int numRead = jw_GetData(m_Device.read[m_ModeType], m_Caps, buffer);


	if (numRead > 0)
	{
		csv.Format(L"%d;%d;%d\n", (buffer[1] | (buffer[2] << 8)), (buffer[3] | (buffer[4] << 8)), (buffer[5] | (buffer[6] << 8)));
		m_File.Write((LPCTSTR)csv, csv.GetLength() * sizeof(TCHAR));

		csv.Format(L"%d;%d;%d\n", (buffer[7] | (buffer[8] << 8)), (buffer[9] | (buffer[10] << 8)), (buffer[11] | (buffer[12] << 8)));
		m_File.Write((LPCTSTR)csv, csv.GetLength() * sizeof(TCHAR));

		csv.Format(L"%d;%d;%d\n", (buffer[13] | (buffer[14] << 8)), (buffer[15] | (buffer[16] << 8)), (buffer[17] | (buffer[18] << 8)));
		m_File.Write((LPCTSTR)csv, csv.GetLength() * sizeof(TCHAR));

		csv.Format(L"%d;%d;%d\n", (buffer[19] | (buffer[20] << 8)), (buffer[21] | (buffer[22] << 8)), (buffer[23] | (buffer[24] << 8)));
		m_File.Write((LPCTSTR)csv, csv.GetLength() * sizeof(TCHAR));

		csv.Format(L"%d;%d;%d\n", (buffer[25] | (buffer[26] << 8)), (buffer[27] | (buffer[28] << 8)), (buffer[29] | (buffer[30] << 8)));
		m_File.Write((LPCTSTR)csv, csv.GetLength() * sizeof(TCHAR));
		
		csv.Format(L"%d;%d;%d\n", (buffer[31] | (buffer[32] << 8)), (buffer[33] | (buffer[34] << 8)), (buffer[35] | (buffer[36] << 8)));
		m_File.Write((LPCTSTR)csv, csv.GetLength() * sizeof(TCHAR));
		
		csv.Format(L"%d;%d;%d\n", (buffer[37] | (buffer[38] << 8)), (buffer[39] | (buffer[40] << 8)), (buffer[41] | (buffer[42] << 8)));
		m_File.Write((LPCTSTR)csv, csv.GetLength() * sizeof(TCHAR));
		
		csv.Format(L"%d;%d;%d\n", (buffer[43] | (buffer[44] << 8)), (buffer[45] | (buffer[46] << 8)), (buffer[47] | (buffer[48] << 8)));
		m_File.Write((LPCTSTR)csv, csv.GetLength() * sizeof(TCHAR));
		
		csv.Format(L"%d;%d;%d\n", (buffer[49] | (buffer[50] << 8)), (buffer[51] | (buffer[52] << 8)), (buffer[53] | (buffer[54] << 8)));
		m_File.Write((LPCTSTR)csv, csv.GetLength() * sizeof(TCHAR));
		
		csv.Format(L"%d;%d;%d\n", (buffer[55] | (buffer[56] << 8)), (buffer[57] | (buffer[58] << 8)), (buffer[59] | (buffer[60] << 8)));
		m_File.Write((LPCTSTR)csv, csv.GetLength() * sizeof(TCHAR));
	}

	m_ProgCount++;

	if (m_ProgCount >= 100)
	{
		m_Progress.StepIt();
		m_ProgCount = 0;
	}
}

void CspeedtestDlg::ChangeBwInput(int type)
{
	m_ACut.ResetContent();

	if (type == 0)
	{
		m_ACut.AddString(L"16.66 Hz");
		m_ACut.AddString(L"8.33 Hz");
		m_ACut.AddString(L"92.56 Hz");
		m_ACut.AddString(L"2.08 Hz");
	}
	else
	{
		m_ACut.AddString(L"208.25 Hz");
		m_ACut.AddString(L"8.33 Hz");
		m_ACut.AddString(L"92.56 Hz");
		m_ACut.AddString(L"2.08 Hz");
	}

}

void CspeedtestDlg::OnCbnSelchangeCombo1()
{
	jw_GetDeviceHandles(CODEMERCS_VID, JW56FR1_PID, m_SerialList[m_DeviceList.GetCurSel()], &m_Device);
	m_Caps = jw_GetHipdCaps(m_Device.read[JW56FR1_ENDPOINT_ROT_SPEED]);
	//jw56fr1_SetParams(m_Device.write[JW56FR1_ENDPOINT_DEFAULT], 0, 0, 1, 0);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
}

void CspeedtestDlg::SetConfig()
{
	BYTE a_range = 0x00;
	BYTE a_filt = 0x00;
	BYTE g_range = 0x00;
	BYTE g_filt = 0x00;


	switch (m_ARange.GetCurSel())
	{
	case 0: a_range = 0x00; break;
	case 1: a_range = 0x02; break;
	case 2: a_range = 0x03; break;
	case 3: a_range = 0x01; break;
	}

	switch (m_ABand.GetCurSel())
	{
	case 0: a_filt |= 0x00; break;
	case 1: a_filt |= 0x20; break;
	}


	switch (m_AFilter.GetCurSel())
	{
	case 0:	a_filt |= 0x00;	break;
	case 1:	a_filt |= 0x40;	break;
	case 2:	a_filt |= 0x80;	break;
	}


	if (m_AFilter.GetCurSel() > 0)
	{
		switch (m_ACut.GetCurSel())
		{
		case 0: a_filt |= 0x00; break;
		case 1: a_filt |= 0x01; break;
		case 2: a_filt |= 0x02; break;
		case 3: a_filt |= 0x03; break;
		}
	}
	else
		a_filt |= 0x00;

	switch (m_RRange.GetCurSel())
	{
	case 0: g_range = 0x01; break;
	case 1: g_range = 0x00; break;
	case 2: g_range = 0x02; break;
	case 3: g_range = 0x04; break;
	case 4: g_range = 0x06; break;
	}

	if (m_RHp.GetCheck() == BST_CHECKED)
		g_filt |= 0x80;
	else
		g_filt |= 0x00;


	switch (m_RCut.GetCurSel())
	{
	case 0: g_filt |= 0x00; break;
	case 1: g_filt |= 0x01; break;
	case 2: g_filt |= 0x02; break;
	case 3: g_filt |= 0x03; break;
	}

	jw56fr1_SetParams(m_Device.write[JW56FR1_ENDPOINT_DEFAULT], a_range, a_filt, g_range, g_filt);
}

void CspeedtestDlg::OnBnClickedButton2()
{
	//Datei anlegen
	CString csv = L"";
	CString temp = L"";
	byte d;
	CFileDialog cDialog(FALSE, 0, 0, 4 | 2, _T("CSV (*.csv) |*.csv||"), 0, 0);

	if (m_IsFile == false)
	{
		if (cDialog.DoModal() == IDOK)
		{
			if (cDialog.GetFileExt() == _T(""))
				m_Path.Format(_T("%s.csv"), cDialog.GetPathName());
			else
				m_Path.Format(_T("%s"), cDialog.GetPathName());

			m_File.Open(m_Path, CFile::modeCreate | CFile::modeWrite);

			//Write header into CSV file

			csv = _T("Setup\n");
			m_File.Write((LPCTSTR)csv, csv.GetLength() * sizeof(TCHAR));

			switch (m_Mode.GetCurSel())
			{
			case 0: //Acc
				csv = _T("Mode=Acceleration\n");
				m_File.Write((LPCTSTR)csv, csv.GetLength() * sizeof(TCHAR));
				
				csv = L"";
				m_ABand.GetLBText(m_ABand.GetCurSel(), temp);
				csv.Append(L"Band=");
				csv.Append(temp);
				csv.Append(L";");

				m_ARange.GetLBText(m_ARange.GetCurSel(), temp);
				csv.Append(L"Range=");
				csv.Append(temp);
				csv.Append(L";");

				m_ACut.GetLBText(m_ACut.GetCurSel(), temp);
				csv.Append(L"Cutoff=");
				csv.Append(temp);
				csv.Append(L";");

				m_AFilter.GetLBText(m_AFilter.GetCurSel(), temp);
				csv.Append(L"Filter=");
				csv.Append(temp);
				csv.Append(L";");

				//Get factor for calculation
				switch (m_ARange.GetCurSel())
				{
				case 0: d = 0x00; break;
				case 1: d = 0x02; break;
				case 2: d = 0x03; break;
				case 3: d = 0x01; break;
				}

				temp.Format(L"%0.3f", jw56fr1_GetAccConversion(d));
				csv.Append(L"Factor=");
				csv.Append(temp);
				csv.Append(L";");

				csv.Append(L"\n");
				break;
			case 1:	//Gyro
				csv = _T("Mode=Gyro\n");
				m_File.Write((LPCTSTR)csv, csv.GetLength() * sizeof(TCHAR));

				csv = L"";
				m_RRange.GetLBText(m_RRange.GetCurSel(), temp);
				csv.Append(L"Range=");
				csv.Append(temp);
				csv.Append(L";");

				m_RCut.GetLBText(m_RCut.GetCurSel(), temp);
				csv.Append(L"Cutoff=");
				csv.Append(temp);
				csv.Append(L";");

				temp.Format(L"HP=%d", m_RHp.GetCheck());
				csv.Append(temp);
				csv.Append(L";");

				//Get factor for calculation
				switch (m_RRange.GetCurSel())
				{
				case 0: d = 0x01; break;
				case 1: d = 0x00; break;
				case 2: d = 0x02; break;
				case 3: d = 0x04; break;
				case 4: d = 0x06; break;
				}

				temp.Format(L"%0.3f", jw56fr1_GetGyroConversion(d));
				csv.Append(L"Factor=");
				csv.Append(temp);
				csv.Append(L";");

				csv.Append(L"\n");
				break;
			default:
				break;
			}

			
			m_File.Write((LPCTSTR)csv, csv.GetLength() * sizeof(TCHAR));

			csv = _T("Data\n");
			m_File.Write((LPCTSTR)csv, csv.GetLength() * sizeof(TCHAR));

			csv = _T("X;Y;Z\n");
			m_File.Write((LPCTSTR)csv, csv.GetLength() * sizeof(TCHAR));

			m_IsFile = true;
		}
	}

	if (m_IsFile == true)
	{
		
		if (m_bRun == false)
		{
			GetDlgItem(IDC_BUTTON2)->SetWindowTextW(L"Stop");
			m_Mode.EnableWindow(FALSE);
			GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);
			m_bRun = true;
			SetConfig();	//Set config
			OnCbnSelchangeCmbMode();	//Get Endpoint
			jw56fr1_SetMode(m_Device.write[JW56FR1_ENDPOINT_DEFAULT], 0x01);
			m_nIDEvent = MM_SetTimer(1);	//Multimediatimer in ms
		}
		else
		{
			//jw_CloseHandle(m_Device.write);
			//jw_CloseHandle(m_Device.read);

			MM_KillTimer(m_nIDEvent);
			jw56fr1_SetMode(m_Device.write[JW56FR1_ENDPOINT_DEFAULT], 0x00);
			GetDlgItem(IDC_BUTTON2)->SetWindowTextW(L"Run");
			GetDlgItem(IDC_COMBO1)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);
			m_Mode.EnableWindow(TRUE);
			m_bRun = false;
			m_IsFile = false;
			m_File.Close();
		}
	}
}


void CspeedtestDlg::OnBnClickedButton4()
{
	int num = jw_GetDeviceList(CODEMERCS_VID, JW56FR1_PID, m_SerialList);

	m_DeviceList.ResetContent();

	for (int i = 0; i < num; i++)
		m_DeviceList.AddString(m_SerialList[i]);

	if (num == 0)
	{
		MessageBoxW(L"NO JW56FR1 found on USB", L"DEVICE ERROR", MB_OK | MB_ICONERROR);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	}

	if (num == 1)
	{
		m_DeviceList.SetCurSel(0);
		OnCbnSelchangeCombo1();
	}
}

void CspeedtestDlg::OnCbnSelchangeCmbMode()
{
	if (m_Mode.GetCurSel() == 0)
		m_ModeType = JW56FR1_ENDPOINT_ACC_SPEED;
	else
		m_ModeType = JW56FR1_ENDPOINT_ROT_SPEED;
}

void CspeedtestDlg::OnCbnSelchangeCombo4()
{
	switch (m_AFilter.GetCurSel())
	{
	case 0:
		m_ACut.EnableWindow(FALSE);
		break;

	case 1:
		m_ACut.EnableWindow(TRUE);
		ChangeBwInput(1);
		m_ACut.SetCurSel(0);
		break;

	case 2:
		m_ACut.EnableWindow(TRUE);
		ChangeBwInput(0);
		m_ACut.SetCurSel(0);
		break;


	}
}

CString CspeedtestDlg::GetVersion()
{
	HMODULE hInst = AfxGetResourceHandle();
	HRSRC hResInfo;
	DWORD dwSize;
	HGLOBAL hResData;
	LPVOID pRes, pResCopy;
	UINT uLen;
	VS_FIXEDFILEINFO *lpFfi;

	CString version = L"";

	hResInfo = FindResource(hInst, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
	dwSize = SizeofResource(hInst, hResInfo);
	hResData = LoadResource(hInst, hResInfo);
	pRes = LockResource(hResData);
	pResCopy = LocalAlloc(LMEM_FIXED, dwSize);
	CopyMemory(pResCopy, pRes, dwSize);
	FreeResource(hResData);

	VerQueryValue(pResCopy, TEXT("\\"), (LPVOID*)&lpFfi, &uLen);

	DWORD dwFileVersionMS = lpFfi->dwFileVersionMS;
	DWORD dwFileVersionLS = lpFfi->dwFileVersionLS;

	DWORD dwLeftMost = HIWORD(dwFileVersionMS);
	DWORD dwSecondLeft = LOWORD(dwFileVersionMS);
	DWORD dwSecondRight = HIWORD(dwFileVersionLS);
	DWORD dwRightMost = LOWORD(dwFileVersionLS);

	LocalFree(pResCopy);

	version.Format(L"%X.%X.%X.%X", dwLeftMost, dwSecondLeft, dwSecondRight, dwRightMost);

	return version;
}
