
// ShowDataDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ShowData.h"
#include "ShowDataDlg.h"
#include "afxdialogex.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CShowDataDlg-Dialogfeld



CShowDataDlg::CShowDataDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SHOWDATA_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShowDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_DEVICE, m_DeviceList);
	DDX_Control(pDX, IDC_CMB_SETTING_ARANGE, m_ARange);
	DDX_Control(pDX, IDC_CMB_SETTING_ACUTOFF, m_ACutoff);
	DDX_Control(pDX, IDC_CMB_SETTING_ABAND, m_ABand);
	DDX_Control(pDX, IDC_CMB_SETTING_GCUTOFF, m_GCutoff);
	DDX_Control(pDX, IDC_CMB_SETTING_GRANGE, m_GRange);
	DDX_Control(pDX, IDC_CHK_GHP, m_GHp);
	DDX_Control(pDX, IDC_CMB_MODE, m_Mode);
	DDX_Control(pDX, IDC_CMB_SETTING_AFILTER, m_AFilter);
}

BEGIN_MESSAGE_MAP(CShowDataDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SEARCH, &CShowDataDlg::OnBnClickedBtnSearch)
	ON_BN_CLICKED(IDC_BTN_RUN, &CShowDataDlg::OnBnClickedBtnRun)
	ON_BN_CLICKED(IDC_BTN_SETTINGS, &CShowDataDlg::OnBnClickedBtnSettings)
	ON_CBN_SELCHANGE(IDC_CMB_DEVICE, &CShowDataDlg::OnCbnSelchangeCmbDevice)
	ON_CBN_SELCHANGE(IDC_CMB_SETTING_AFILTER, &CShowDataDlg::OnCbnSelchangeCmbSettingAfilter)
	ON_BN_CLICKED(IDC_BTN_STORE, &CShowDataDlg::OnBnClickedBtnStore)
END_MESSAGE_MAP()


// CShowDataDlg-Meldungshandler

BOOL CShowDataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Groﬂes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	m_bRun = false;

	m_ARange.SetCurSel(0);
	m_AFilter.SetCurSel(0);
	m_ABand.SetCurSel(0);
	m_ACutoff.SetCurSel(0);
	m_GRange.SetCurSel(0);
	m_GCutoff.SetCurSel(0);

	m_ACutoff.EnableWindow(FALSE);

	m_Mode.SetCurSel(0);


	m_Conversion.acc = JW56FR1_CONVERSION_ACC_2G;
	m_Conversion.rot = JW56FR1_CONVERSION_GYRO_245;

	return TRUE;  
}

void CShowDataDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Ger‰tekontext zum Zeichnen

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

HCURSOR CShowDataDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





void CALLBACK CShowDataDlg::MM_TimerFunction(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	CShowDataDlg* obj = (CShowDataDlg*)dwUser;
	obj->MM_TimerHandler(wTimerID);
}

UINT CShowDataDlg::MM_SetTimer(UINT ms)
{
	UINT eventID = 0;
	TIMECAPS tc;
	
	timeGetDevCaps(&tc, sizeof(TIMECAPS));

	DWORD resolution = min(max(tc.wPeriodMin, 0), tc.wPeriodMax);
	timeBeginPeriod(resolution);

	eventID = timeSetEvent(ms, resolution, MM_TimerFunction, (DWORD)this, TIME_PERIODIC);
	return eventID;
}

void CShowDataDlg::MM_KillTimer(UINT nIDEvent)
{
	timeKillEvent(nIDEvent);
}

void CShowDataDlg::MM_TimerHandler(UINT nIDEvent)
{
	BYTE buffer[JW56FR1_READ_SIZE_DEFAULT] = { 0 };
	WORD x, y, z = 0;
	WORD rx, ry, rz = 0;
	CString values;


	HIDP_CAPS caps = jw_GetHipdCaps(m_Device.read[0]);
	int numRead = jw_GetData(m_Device.read[0], caps, buffer);


	if (numRead > 0)
	{
		x = (buffer[1] | (buffer[2] << 8));
		y = (buffer[3] | (buffer[4] << 8));
		z = (buffer[5] | (buffer[6] << 8));

		rx = (buffer[7] | (buffer[8] << 8));
		ry = (buffer[9] | (buffer[10] << 8));
		rz = (buffer[11] | (buffer[12] << 8));

		switch (m_Mode.GetCurSel())
		{
		case 0:
			values.Format(L"%d", x);
			GetDlgItem(IDC_STATIC_ACCX)->SetWindowTextW(values);

			values.Format(L"%d", y);
			GetDlgItem(IDC_STATIC_ACCY)->SetWindowTextW(values);

			values.Format(L"%d", z);
			GetDlgItem(IDC_STATIC_ACCZ)->SetWindowTextW(values);

			values.Format(L"%d", rx);
			GetDlgItem(IDC_STATIC_GYRX)->SetWindowTextW(values);

			values.Format(L"%d", ry);
			GetDlgItem(IDC_STATIC_GYRY)->SetWindowTextW(values);

			values.Format(L"%d", rz);
			GetDlgItem(IDC_STATIC_GYRZ)->SetWindowTextW(values);
			break;
		
		case 1:
			values.Format(L"%d", x - JW56FR1_OFFSET);
			GetDlgItem(IDC_STATIC_ACCX)->SetWindowTextW(values);

			values.Format(L"%d", y - JW56FR1_OFFSET);
			GetDlgItem(IDC_STATIC_ACCY)->SetWindowTextW(values);

			values.Format(L"%d", z - JW56FR1_OFFSET);
			GetDlgItem(IDC_STATIC_ACCZ)->SetWindowTextW(values);

			values.Format(L"%d", rx - JW56FR1_OFFSET);
			GetDlgItem(IDC_STATIC_GYRX)->SetWindowTextW(values);

			values.Format(L"%d", ry - JW56FR1_OFFSET);
			GetDlgItem(IDC_STATIC_GYRY)->SetWindowTextW(values);

			values.Format(L"%d", rz - JW56FR1_OFFSET);
			GetDlgItem(IDC_STATIC_GYRZ)->SetWindowTextW(values);
			break;

		case 2:
			values.Format(L"%0.3f mG", (x - JW56FR1_OFFSET) * m_Conversion.acc);
			GetDlgItem(IDC_STATIC_ACCX)->SetWindowTextW(values);

			values.Format(L"%0.3f mG", (y - JW56FR1_OFFSET) * m_Conversion.acc);
			GetDlgItem(IDC_STATIC_ACCY)->SetWindowTextW(values);

			values.Format(L"%0.3f mG", (z - JW56FR1_OFFSET) * m_Conversion.acc);
			GetDlgItem(IDC_STATIC_ACCZ)->SetWindowTextW(values);

			values.Format(L"%0.0f mDps", (rx - JW56FR1_OFFSET) * m_Conversion.rot);
			GetDlgItem(IDC_STATIC_GYRX)->SetWindowTextW(values);

			values.Format(L"%0.0f mDps", (ry - JW56FR1_OFFSET) * m_Conversion.rot);
			GetDlgItem(IDC_STATIC_GYRY)->SetWindowTextW(values);

			values.Format(L"%0.0f mDps", (rz - JW56FR1_OFFSET) * m_Conversion.rot);
			GetDlgItem(IDC_STATIC_GYRZ)->SetWindowTextW(values);
			break;
		}
	
	}
}


void CShowDataDlg::ChangeBwInput(int type)
{
	m_ACutoff.ResetContent();

	if (type == 0)
	{
		m_ACutoff.AddString(L"16.66 Hz");
		m_ACutoff.AddString(L"8.33 Hz");
		m_ACutoff.AddString(L"92.56 Hz");
		m_ACutoff.AddString(L"2.08 Hz");
	}
	else
	{
		m_ACutoff.AddString(L"208.25 Hz");
		m_ACutoff.AddString(L"8.33 Hz");
		m_ACutoff.AddString(L"92.56 Hz");
		m_ACutoff.AddString(L"2.08 Hz");
	}

}


void CShowDataDlg::WriteConfig()
{
	BYTE a_range = 0x00;
	BYTE a_filt = 0x00;
	BYTE g_range = 0x00;
	BYTE g_filt = 0x00;


	switch (m_ARange.GetCurSel())
	{
	case 0:
		a_range = 0x00;
		m_Conversion.acc = JW56FR1_CONVERSION_ACC_2G;
		break;
	case 1:
		a_range = 0x02;
		m_Conversion.acc = JW56FR1_CONVERSION_ACC_4G;
		break;
	case 2:
		a_range = 0x03;
		m_Conversion.acc = JW56FR1_CONVERSION_ACC_8G;
		break;
	case 3:
		a_range = 0x01;
		m_Conversion.acc = JW56FR1_CONVERSION_ACC_16G;
		break;
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
		switch (m_ACutoff.GetCurSel())
		{
		case 0: a_filt |= 0x00; break;
		case 1: a_filt |= 0x01; break;
		case 2: a_filt |= 0x02; break;
		case 3: a_filt |= 0x03; break;
		}
	}
	else
		a_filt |= 0x00;

	switch (m_GRange.GetCurSel())
	{
	case 0:
		g_range = 0x01;
		m_Conversion.rot = JW56FR1_CONVERSION_GYRO_125;
		break;
	case 1:
		g_range = 0x00;
		m_Conversion.rot = JW56FR1_CONVERSION_GYRO_245;
		break;
	case 2:
		g_range = 0x02;
		m_Conversion.rot = JW56FR1_CONVERSION_GYRO_500;
		break;
	case 3:
		g_range = 0x04;
		m_Conversion.rot = JW56FR1_CONVERSION_GYRO_1000;
		break;
	case 4:
		g_range = 0x06;
		m_Conversion.rot = JW56FR1_CONVERSION_GYRO_2000;
		break;
	}

	if (m_GHp.GetCheck() == BST_CHECKED)
		g_filt |= 0x80;
	else
		g_filt |= 0x00;


	switch (m_GCutoff.GetCurSel())
	{
	case 0: g_filt |= 0x00; break;
	case 1: g_filt |= 0x01; break;
	case 2: g_filt |= 0x02; break;
	case 3: g_filt |= 0x03; break;
	}

	jw56fr1_SetParams(m_Device.write[0], a_range, a_filt, g_range, g_filt);
}



void CShowDataDlg::OnBnClickedBtnSettings()
{
	WriteConfig();

}

void CShowDataDlg::OnBnClickedBtnStore()
{
	WriteConfig();
	jw56fr1_StoreParams(m_Device.write[0]);
}


void CShowDataDlg::OnBnClickedBtnSearch()
{
	int num = jw_GetDeviceList(CODEMERCS_VID, JW56FR1_PID, m_SerialList);

	m_DeviceList.ResetContent();

	for (int i = 0; i < num; i++)
		m_DeviceList.AddString(m_SerialList[i]);

	if (num == 0)
	{
		MessageBoxW(L"NO JW56FR1 found on USB", L"DEVICE ERROR", MB_OK | MB_ICONERROR);
		GetDlgItem(IDC_BTN_RUN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_SETTINGS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_STORE)->EnableWindow(FALSE);
	}
		
	else if (num == 1)
	{
		m_DeviceList.SetCurSel(0);
		jw_GetDeviceHandles(CODEMERCS_VID, JW56FR1_PID, m_SerialList[0], &m_Device);
		GetDlgItem(IDC_BTN_RUN)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_SETTINGS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_STORE)->EnableWindow(TRUE);
	}
		
}


void CShowDataDlg::OnBnClickedBtnRun()
{
	if (m_bRun == false)
	{
		//jw56fr1_SetParams(m_Device.write[0], JW56FR1_ARNG_RANGE_2G, JW56FR1_AFILT_NONE, JW56FR1_GRNG_125DPS, JW56FR1_GFILT_NONE);

		GetDlgItem(IDC_BTN_RUN)->SetWindowTextW(L"Stop");
		m_bRun = true;
		m_nIDEvent = MM_SetTimer(50);	//Multimediatimer in ms
	}
	else
	{
		//jw_CloseHandle(m_Device.write);
		//jw_CloseHandle(m_Device.read);

		MM_KillTimer(m_nIDEvent);
		GetDlgItem(IDC_BTN_RUN)->SetWindowTextW(L"Run");
		m_bRun = false;
	}
}


void CShowDataDlg::OnCbnSelchangeCmbDevice()
{
	jw_GetDeviceHandles(CODEMERCS_VID, JW56FR1_PID, m_SerialList[m_DeviceList.GetCurSel()], &m_Device);
	jw56fr1_SetParams(m_Device.write[0], JW56FR1_ARNG_RANGE_2G, JW56FR1_AFILT_BAND_0, JW56FR1_GRNG_245DPS, JW56FR1_AFILT_BAND_0);

	GetDlgItem(IDC_BTN_RUN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SETTINGS)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_STORE)->EnableWindow(TRUE);
}


void CShowDataDlg::OnCbnSelchangeCmbSettingAfilter()
{
	switch (m_AFilter.GetCurSel())
	{
	case 0:
		m_ACutoff.EnableWindow(FALSE);
		break;

	case 1:
		m_ACutoff.EnableWindow(TRUE);
		ChangeBwInput(1);
		m_ACutoff.SetCurSel(0);
		break;

	case 2:
		m_ACutoff.EnableWindow(TRUE);
		ChangeBwInput(0);
		m_ACutoff.SetCurSel(0);
		break;


	}
}




