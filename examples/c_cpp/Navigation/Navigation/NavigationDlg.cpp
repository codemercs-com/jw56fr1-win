
// NavigationDlg.cpp: Implementierungsdatei
//

#include "pch.h"
#include "framework.h"
#include "Navigation.h"
#include "NavigationDlg.h"
#include "afxdialogex.h"
#include <mmsystem.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define THREAD_START 1
#define THREAD_STOP 0


// CNavigationDlg-Dialogfeld



CNavigationDlg::CNavigationDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NAVIGATION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNavigationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ACC_X, m_StaticRawAcc.x);
	DDX_Control(pDX, IDC_STATIC_ACC_Y, m_StaticRawAcc.y);
	DDX_Control(pDX, IDC_STATIC_ACC_Z, m_StaticRawAcc.z);
	DDX_Control(pDX, IDC_STATIC_GFORCE_X, m_StaticAcceleration.x);
	DDX_Control(pDX, IDC_STATIC_GFORCE_Y, m_StaticAcceleration.y);
	DDX_Control(pDX, IDC_STATIC_GFORCE_Z, m_StaticAcceleration.z);
	DDX_Control(pDX, IDC_STATIC_COMP_X, m_StaticComp.x);
	DDX_Control(pDX, IDC_STATIC_COMP_Y, m_StaticComp.y);
	DDX_Control(pDX, IDC_STATIC_COMP_Z, m_StaticComp.z);
	DDX_Control(pDX, IDC_STATIC_SPEED_X, m_StaticSpeed.x);
	DDX_Control(pDX, IDC_STATIC_SPEED_Y, m_StaticSpeed.y);
	DDX_Control(pDX, IDC_STATIC_SPEED_Z, m_StaticSpeed.z);
	DDX_Control(pDX, IDC_STATIC_TRAVEL_X, m_StaticTravel.x);
	DDX_Control(pDX, IDC_STATIC_TRAVEL_Y, m_StaticTravel.y);
	DDX_Control(pDX, IDC_STATIC_TRAVEL_Z, m_StaticTravel.z);
	DDX_Control(pDX, IDC_CMB_ACC_BW, m_ABand);
	DDX_Control(pDX, IDC_CMB_ACC_FILTER, m_AFilter);
	DDX_Control(pDX, IDC_CMB_ACC_CUT, m_ACutOff);
	DDX_Control(pDX, IDC_LIST, m_List);
}

BEGIN_MESSAGE_MAP(CNavigationDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CNavigationDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CNavigationDlg::OnBnClickedButtonRun)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CNavigationDlg::OnBnClickedButtonReset)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_INIT, &CNavigationDlg::OnBnClickedButtonInit)
	ON_BN_CLICKED(IDC_BUTTON_AFILTER, &CNavigationDlg::OnBnClickedButtonAfilter)
	ON_CBN_SELCHANGE(IDC_CMB_ACC_FILTER, &CNavigationDlg::OnCbnSelchangeCmbAccFilter)
END_MESSAGE_MAP()


// CNavigationDlg-Meldungshandler

BOOL CNavigationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Gro√ües Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	m_Offset.x = 0;
	m_Offset.y = 0;
	m_Offset.z = 0;

	m_AFilter.SetCurSel(0);
	m_ABand.SetCurSel(0);
	m_ACutOff.SetCurSel(0);

	m_ThreadData.run = false;
	m_ThreadData.status = THREAD_START;

	return TRUE;
}


void CNavigationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Ger√§tekontext zum Zeichnen

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

HCURSOR CNavigationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CNavigationDlg::PumpMsgQueue()
{
	// Pump the message queue
	MSG   msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		AfxGetThread()->PumpMessage();
}

void CNavigationDlg::OnBnClickedButtonConnect()
{
	if (m_Joywarrior.Open())
	{
		//Set up the JoyWarrior56FR1
		m_Joywarrior.SetMode(JW56FR1_MODE_NORMAL);
		m_Joywarrior.SetParameter(JW56FR1_ARNG_RANGE_2G, JW56FR1_AFILT_NONE, JW56FR1_GRNG_245DPS, JW56FR1_GFILT_NONE); //Set JW56FR1 to fefault

		GetDlgItem(IDC_BUTTON_INIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_AFILTER)->EnableWindow(TRUE);
	}
}

void CNavigationDlg::OnCbnSelchangeCmbAccFilter()
{
	switch (m_AFilter.GetCurSel())
	{
	case 0:
		GetDlgItem(IDC_CMB_ACC_CUT)->EnableWindow(FALSE);
		break;

	case 1:
		GetDlgItem(IDC_CMB_ACC_CUT)->EnableWindow(TRUE);
		ChangeBwInput(1);
		m_ACutOff.SetCurSel(0);
		break;

	case 2:
		GetDlgItem(IDC_CMB_ACC_CUT)->EnableWindow(TRUE);
		ChangeBwInput(0);
		m_ACutOff.SetCurSel(0);
		break;
	}
}

void CNavigationDlg::ChangeBwInput(int type)
{
	m_ACutOff.ResetContent();

	if (type == 0)
	{
		m_ACutOff.AddString(L"16.66 Hz");
		m_ACutOff.AddString(L"8.33 Hz");
		m_ACutOff.AddString(L"92.56 Hz");
		m_ACutOff.AddString(L"2.08 Hz");
	}
	else
	{
		m_ACutOff.AddString(L"208.25 Hz");
		m_ACutOff.AddString(L"8.33 Hz");
		m_ACutOff.AddString(L"92.56 Hz");
		m_ACutOff.AddString(L"2.08 Hz");
	}
}

void CNavigationDlg::OnBnClickedButtonAfilter()
{
	BYTE a_filt = 0x00;

	switch (m_ABand.GetCurSel())
	{
	case 0: a_filt |= JW56FR1_AFILT_BAND_0; break;
	case 1: a_filt |= JW56FR1_AFILT_BAND_1; break;
	}

	switch (m_AFilter.GetCurSel())
	{
	case 0:	a_filt |= JW56FR1_AFILT_LP1_ONLY; break;
	case 1:	a_filt |= JW56FR1_AFILT_LP1_HP; break;
	case 2:	a_filt |= JW56FR1_AFILT_LP1_LP2; break;
	}

	if (m_AFilter.GetCurSel() > 0)
	{
		switch (m_ACutOff.GetCurSel())
		{
		case 0: a_filt |= 0x00; break;
		case 1: a_filt |= 0x01; break;
		case 2: a_filt |= 0x02; break;
		case 3: a_filt |= 0x03; break;
		}
	}
	else
		a_filt |= 0x00;

	m_Joywarrior.SetParameter(JW56FR1_ARNG_RANGE_2G, a_filt, JW56FR1_GRNG_245DPS, JW56FR1_GFILT_NONE);
}

void CNavigationDlg::OnBnClickedButtonRun()
{
	if (m_bTimer == false)
	{
		m_Acceleration.x = 0;
		m_Acceleration.y = 0;
		m_Acceleration.z = 0;

		m_Speed.x = 0;
		m_Speed.y = 0;
		m_Speed.z = 0;

		//Start multimedia timer with 100ms interval for update dialog with thread data
		m_idEvent = SetMMTimer(100);
		m_bTimer = true;
		GetDlgItem(IDC_BUTTON_RUN)->SetWindowTextW(L"Stop");

		if (m_ThreadData.run == false)
		{
			CWinThread* pThreadData = AfxBeginThread(Thread_Data, (LPVOID)this, THREAD_PRIORITY_NORMAL, (UINT)0, (DWORD)0, (LPSECURITY_ATTRIBUTES)NULL);

			if (!pThreadData)
				OutputDebugString(L"AfxBeginThread() Failed!");
			else
			{
				m_ThreadData.status = THREAD_START;
				m_ThreadData.run = true;
			}
		}
	}
	else
	{
		//Stop multimedia timer and clear
		KillMMTimer(m_idEvent);
		m_bTimer = false;
		m_idEvent = 0;
		m_ThreadData.status = THREAD_STOP;
		m_ThreadData.run = false;
		GetDlgItem(IDC_BUTTON_RUN)->SetWindowTextW(L"Start");
	}
}

void CNavigationDlg::OnBnClickedButtonReset()
{
	memset(&m_Acceleration, 0, sizeof(SpeedStruct));
	memset(&m_Speed, 0, sizeof(SpeedStruct));
	memset(&m_Travel, 0, sizeof(SpeedStruct));
}

void CNavigationDlg::OnClose()
{
	m_Joywarrior.Close();
	CDialogEx::OnClose();
}


void CALLBACK CNavigationDlg::TimerFunction(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	CNavigationDlg* obj = (CNavigationDlg*)dwUser;
	obj->MMTimerHandler(wTimerID);
}

UINT CNavigationDlg::SetMMTimer(UINT ms)
{
	TIMECAPS tc;
	timeGetDevCaps(&tc, sizeof(TIMECAPS));

	DWORD resolution = min(max(tc.wPeriodMin, 0), tc.wPeriodMax);
	timeBeginPeriod(resolution);

	m_idEvent = timeSetEvent(ms, resolution, TimerFunction, (DWORD)this, TIME_PERIODIC);
	m_bTimer = TRUE;

	return m_idEvent;
}

void CNavigationDlg::KillMMTimer(UINT nIDEvent)
{
	timeKillEvent(nIDEvent);
	m_idEvent = 0;
	m_bTimer = FALSE;
}

void CNavigationDlg::MMTimerHandler(UINT nIDEvent)
{
	PumpMsgQueue();
	CString text;

	text.Format(L"%d", m_AccData.x);
	m_StaticRawAcc.x.SetWindowTextW(text);
	text.Format(L"%d", m_AccData.y);
	m_StaticRawAcc.y.SetWindowTextW(text);
	text.Format(L"%d", m_AccData.z);
	m_StaticRawAcc.z.SetWindowTextW(text);
	
	text.Format(L"%0.3f G | %0.2f m/s≤", m_SpeedData.x, m_Acceleration.x);
	m_StaticAcceleration.x.SetWindowTextW(text);
	text.Format(L"%0.3f G | %0.2f m/s≤", m_SpeedData.y, m_Acceleration.y);
	m_StaticAcceleration.y.SetWindowTextW(text);
	text.Format(L"%0.3f G | %0.2f m/s≤", m_SpeedData.z, m_Acceleration.z);
	m_StaticAcceleration.z.SetWindowTextW(text);
	
	text.Format(L"%0.2f m/s", m_Speed.x);
	m_StaticSpeed.x.SetWindowTextW(text);
	text.Format(L"%0.2f m/s", m_Speed.y);
	m_StaticSpeed.y.SetWindowTextW(text);
	text.Format(L"%0.2f m/s", m_Speed.z);
	m_StaticSpeed.z.SetWindowTextW(text);

	text.Format(L"%0.2f m", m_Travel.x);
	m_StaticTravel.x.SetWindowTextW(text);
	text.Format(L"%0.2f m", m_Travel.y);
	m_StaticTravel.y.SetWindowTextW(text);
	text.Format(L"%0.2f m", m_Travel.z);
	m_StaticTravel.z.SetWindowTextW(text);
}

void CNavigationDlg::OnBnClickedButtonInit()
{
	JW56FR1_DATA data;
	CString text;

	m_Offset.x = 0;
	m_Offset.y = 0;
	m_Offset.z = 0;

	//Get the offset of each axis to get close to ZERO
	for (int i = 0; i < 255; i++)
	{
		data = m_Joywarrior.GetData();
		m_Offset.x = m_Offset.x + data.accX;
		m_Offset.y = m_Offset.y + data.accY;
		m_Offset.z = m_Offset.z + data.accZ;
	}

	//Get mid value
	m_Offset.x /= 255;
	m_Offset.y /= 255;
	m_Offset.z /= 255;

	//Clean output
	m_Offset.x = m_Offset.x - JW56FR1_ZERO;
	m_Offset.y = m_Offset.y - JW56FR1_ZERO;
	m_Offset.z = m_Offset.z - JW56FR1_ZERO + JW56FR1_2G;

	text.Format(L"%d", m_Offset.x);
	m_StaticComp.x.SetWindowTextW(text);
	text.Format(L"%d", m_Offset.y);
	m_StaticComp.y.SetWindowTextW(text);
	text.Format(L"%d", m_Offset.z);
	m_StaticComp.z.SetWindowTextW(text);
}


UINT CNavigationDlg::Thread_Data(LPVOID pParam)
{
	CNavigationDlg* pObject = (CNavigationDlg*)pParam;

	if (pObject == NULL)
		return 1;

	timeBeginPeriod(1); //Get the fastest timing

	JW56FR1_DATA data;

	//Get some data and dump them
	for(int i=0; i<100; i++)
		pObject->m_Joywarrior.GetData();

	for (;;)
	{
		//Exit Thread from extern event
		if (pObject->m_ThreadData.status == THREAD_STOP)
			break;

		data = pObject->m_Joywarrior.GetData();

		//Get acceleration values for X, Y, Z without joystick offset and offset of the sensor as RAW data
		pObject->m_AccData.x = data.accX - JW56FR1_ZERO - pObject->m_Offset.x;
		pObject->m_AccData.y = data.accY - JW56FR1_ZERO - pObject->m_Offset.y;
		pObject->m_AccData.z = data.accZ - JW56FR1_ZERO - pObject->m_Offset.z;

		//Get Acceleration as G-Force and m/s≤
		pObject->m_Acceleration.x = (pObject->m_AccData.x * JW56FR1_G_MS / JW56FR1_2G);
		pObject->m_Acceleration.y = (pObject->m_AccData.y * JW56FR1_G_MS / JW56FR1_2G);
		pObject->m_Acceleration.z = (pObject->m_AccData.z * JW56FR1_G_MS / JW56FR1_2G);

		pObject->m_SpeedData.x = abs((pObject->m_AccData.x * JW56FR1_CONVERSION_ACC_2G) / 1000);
		pObject->m_SpeedData.y = abs((pObject->m_AccData.y * JW56FR1_CONVERSION_ACC_2G) / 1000);
		pObject->m_SpeedData.z = abs((pObject->m_AccData.z * JW56FR1_CONVERSION_ACC_2G) / 1000);

		pObject->m_Speed.x = pObject->m_Speed.x + ((data.accX - JW56FR1_ZERO - pObject->m_Offset.x) * JW56FR1_G_MS / JW56FR1_NORMAL_SIZE / JW56FR1_2G);
		pObject->m_Speed.y = pObject->m_Speed.y + ((data.accY - JW56FR1_ZERO - pObject->m_Offset.y) * JW56FR1_G_MS / JW56FR1_NORMAL_SIZE / JW56FR1_2G);
		pObject->m_Speed.z = pObject->m_Speed.z + (((data.accZ + JW56FR1_2G) - JW56FR1_ZERO - pObject->m_Offset.z) * JW56FR1_G_MS / JW56FR1_NORMAL_SIZE / JW56FR1_2G);

		pObject->m_Travel.x = pObject->m_Travel.x + (pObject->m_Speed.x / JW56FR1_NORMAL_SIZE);
		pObject->m_Travel.y = pObject->m_Travel.y + (pObject->m_Speed.y / JW56FR1_NORMAL_SIZE);
		pObject->m_Travel.z = pObject->m_Travel.z + (pObject->m_Speed.z / JW56FR1_NORMAL_SIZE);
	}

	pObject->m_ThreadData.run = 0;

	return 0;
}
