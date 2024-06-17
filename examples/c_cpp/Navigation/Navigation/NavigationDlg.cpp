
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
}

BEGIN_MESSAGE_MAP(CNavigationDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CNavigationDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CNavigationDlg::OnBnClickedButtonRun)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CNavigationDlg::OnBnClickedButtonReset)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_INIT, &CNavigationDlg::OnBnClickedButtonInit)
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
		m_Joywarrior.SetParameter(JW56FR1_ARNG_RANGE_2G, JW56FR1_AFILT_NONE, JW56FR1_GRNG_245DPS, JW56FR1_GFILT_NONE);

		GetDlgItem(IDC_BUTTON_INIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(TRUE);
	}
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

		//Start multimedia timer with 2ms interval
		m_idEvent = SetMMTimer(1);
		m_bTimer = true;
		GetDlgItem(IDC_BUTTON_RUN)->SetWindowTextW(L"Stop");
	}
	else
	{
		//Stop multimedia timer
		KillMMTimer(m_idEvent);
		m_bTimer = false;
		m_idEvent = 0;
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
	JW56FR1_DATA data = m_Joywarrior.GetData();		//Get data from JW56FR1 as struct

	//Get acceleration values for X, Y, Z without joystick offset and offset of the sensor as RAW data
	int x = data.accX - JW56FR1_ZERO - m_Offset.x;
	int y = data.accY - JW56FR1_ZERO - m_Offset.y;
	int z = data.accZ - JW56FR1_ZERO - m_Offset.z;

	text.Format(L"%d", x);
	m_StaticRawAcc.x.SetWindowTextW(text);
	text.Format(L"%d", y);
	m_StaticRawAcc.y.SetWindowTextW(text);
	text.Format(L"%d", z);
	m_StaticRawAcc.z.SetWindowTextW(text);
	
	//Get Acceleration as G-Force and m/s≤
	m_Acceleration.x = (x * JW56FR1_G_MS / JW56FR1_2G);
	m_Acceleration.y = (y * JW56FR1_G_MS / JW56FR1_2G);
	m_Acceleration.z = (z * JW56FR1_G_MS / JW56FR1_2G);

	double gX = abs((x * JW56FR1_CONVERSION_ACC_2G) / 1000);
	double gY = abs((y * JW56FR1_CONVERSION_ACC_2G) / 1000);
	double gZ = abs((z * JW56FR1_CONVERSION_ACC_2G) / 1000);

	text.Format(L"%0.3f G | %0.2f m/s≤", gX, m_Acceleration.x);
	m_StaticAcceleration.x.SetWindowTextW(text);
	text.Format(L"%0.3f G | %0.2f m/s≤", gY, m_Acceleration.y);
	m_StaticAcceleration.y.SetWindowTextW(text);
	text.Format(L"%0.3f G | %0.2f m/s≤", gZ, m_Acceleration.z);
	m_StaticAcceleration.z.SetWindowTextW(text);


	//Calculate the Speed for each axis
	m_Speed.x = m_Speed.x + ((data.accX - JW56FR1_ZERO) * JW56FR1_G_MS / JW56FR1_NORMAL_SIZE / JW56FR1_2G);
	m_Speed.y = m_Speed.y + ((data.accY - JW56FR1_ZERO) * JW56FR1_G_MS / JW56FR1_NORMAL_SIZE / JW56FR1_2G);
	m_Speed.z = m_Speed.z + (((data.accZ + JW56FR1_2G) - JW56FR1_ZERO) * JW56FR1_G_MS / JW56FR1_NORMAL_SIZE / JW56FR1_2G);
	
	text.Format(L"%0.2f m/s", m_Speed.x);
	m_StaticSpeed.x.SetWindowTextW(text);
	text.Format(L"%0.2f m/s", m_Speed.y);
	m_StaticSpeed.y.SetWindowTextW(text);
	text.Format(L"%0.2f m/s", m_Speed.z);
	m_StaticSpeed.z.SetWindowTextW(text);


	//Calculate travel distance for each axis
	m_Travel.x = m_Travel.x + (m_Speed.x / JW56FR1_NORMAL_SIZE);
	m_Travel.y = m_Travel.y + (m_Speed.y / JW56FR1_NORMAL_SIZE);
	m_Travel.z = m_Travel.z + (m_Speed.z / JW56FR1_NORMAL_SIZE);

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
