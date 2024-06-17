
// zero_offsetDlg.cpp: Implementierungsdatei
//

#include "pch.h"
#include "framework.h"
#include "zero_offset.h"
#include "zero_offsetDlg.h"
#include "afxdialogex.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CzerooffsetDlg::CzerooffsetDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ZERO_OFFSET_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CzerooffsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_X, m_StaticX1);
	DDX_Control(pDX, IDC_STATIC_Y, m_StaticY1);
	DDX_Control(pDX, IDC_STATIC_Z, m_StaticZ1);
	DDX_Control(pDX, IDC_STATIC_X2, m_StaticX2);
	DDX_Control(pDX, IDC_STATIC_Y2, m_StaticY2);
	DDX_Control(pDX, IDC_STATIC_Z2, m_StaticZ2);
	DDX_Control(pDX, IDC_LIST_MSG, m_ListMsg);
}

BEGIN_MESSAGE_MAP(CzerooffsetDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OFFSET, &CzerooffsetDlg::OnBnClickedButtonOffset)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CzerooffsetDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_START, &CzerooffsetDlg::OnBnClickedButtonStart)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CzerooffsetDlg::OnBnClickedButtonClear)
END_MESSAGE_MAP()


// CzerooffsetDlg-Meldungshandler

BOOL CzerooffsetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_bTimer = FALSE;
	m_bOffset = FALSE;
	m_idEvent = 0;

	m_OffsetData.accX = 0;
	m_OffsetData.accY = 0;
	m_OffsetData.accZ = 0;

	return TRUE;  // TRUE zurückgeben, wenn der Fokus nicht auf ein Steuerelement gesetzt wird
}

void CzerooffsetDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void CzerooffsetDlg::OnPaint()
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

HCURSOR CzerooffsetDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CALLBACK CzerooffsetDlg::TimerFunction(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	CzerooffsetDlg* obj = (CzerooffsetDlg*)dwUser;
	obj->MMTimerHandler(wTimerID);
}

UINT CzerooffsetDlg::SetMMTimer(UINT ms)
{
	TIMECAPS tc;
	timeGetDevCaps(&tc, sizeof(TIMECAPS));

	DWORD resolution = min(max(tc.wPeriodMin, 0), tc.wPeriodMax);
	timeBeginPeriod(resolution);

	m_idEvent = timeSetEvent(ms, resolution, TimerFunction, (DWORD)this, TIME_PERIODIC);
	m_bTimer = TRUE;

	return m_idEvent;
}

void CzerooffsetDlg::KillMMTimer(UINT nIDEvent)
{
	timeKillEvent(nIDEvent);
	m_idEvent = 0;
	m_bTimer = FALSE;
}

void CzerooffsetDlg::MMTimerHandler(UINT nIDEvent)
{
	CString text;
	JW56FR1_DATA data;
	data = m_Joywarrior.GetData();

	//Get Offset
	if (m_bOffset == TRUE)
	{
		m_OffsetCount++;
		m_OffsetData.accX = m_OffsetData.accX + data.accX;
		m_OffsetData.accY = m_OffsetData.accY + data.accY;
		m_OffsetData.accZ = m_OffsetData.accZ + data.accZ;

		if (m_OffsetCount % 10 == 0)
		{
			m_ListMsg.AddString(L"Get offset, please wait....");
		}

		//Take 100 values for get offset
		if (m_OffsetCount >= 100)
		{
			m_OffsetData.accX /= m_OffsetCount;
			m_OffsetData.accY /= m_OffsetCount;
			m_OffsetData.accZ /= m_OffsetCount;

			text.Format(L"m_OffsetData.accX = %d\n", m_OffsetData.accX);
			OutputDebugString(text);

			text.Format(L"m_OffsetData.accY = %d\n", m_OffsetData.accY);
			OutputDebugString(text);

			text.Format(L"m_OffsetData.accZ = %d\n", m_OffsetData.accZ);
			OutputDebugString(text);

			m_OffsetData.accX = m_OffsetData.accX - JW56FR1_ZERO;
			m_OffsetData.accY = m_OffsetData.accY - JW56FR1_ZERO;
			m_OffsetData.accZ = m_OffsetData.accZ - JW56FR1_ZERO + JW56FR1_2G;

			m_ListMsg.ResetContent();
			m_ListMsg.AddString(L"---Offset---");
			text.Format(L"X: %d", m_OffsetData.accX);
			m_ListMsg.AddString(text);
			text.Format(L"Y: %d", m_OffsetData.accY);
			m_ListMsg.AddString(text);
			text.Format(L"Z: %d", m_OffsetData.accZ);
			m_ListMsg.AddString(text);

			m_bOffset = FALSE;
		}
	}
	else
	{
		//Output data RAW
		text.Format(L"%d", data.accX - JW56FR1_ZERO);
		m_StaticX1.SetWindowTextW(text);

		text.Format(L"%d", data.accY - JW56FR1_ZERO);
		m_StaticY1.SetWindowTextW(text);

		text.Format(L"%d", data.accZ - JW56FR1_ZERO);
		m_StaticZ1.SetWindowTextW(text);

		//Output data with offset
		text.Format(L"%d", data.accX - JW56FR1_ZERO - m_OffsetData.accX);
		m_StaticX2.SetWindowTextW(text);

		text.Format(L"%d", data.accY - JW56FR1_ZERO - m_OffsetData.accY);
		m_StaticY2.SetWindowTextW(text);

		text.Format(L"%d", data.accZ - JW56FR1_ZERO - m_OffsetData.accZ);
		m_StaticZ2.SetWindowTextW(text);
	}
}

void CzerooffsetDlg::OnBnClickedButtonConnect()
{
	if (m_Joywarrior.Open())
	{
		m_Joywarrior.SetMode(JW56FR1_MODE_NORMAL);
		m_Joywarrior.SetParameter(JW56FR1_ARNG_RANGE_2G, JW56FR1_AFILT_NONE, JW56FR1_GRNG_245DPS, JW56FR1_GFILT_NONE); //Start-up values from JW56FR1

		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_OFFSET)->EnableWindow(FALSE);
	}
}

void CzerooffsetDlg::OnBnClickedButtonOffset()
{
	m_OffsetData.accX = 0;
	m_OffsetData.accY = 0;
	m_OffsetData.accZ = 0;

	m_OffsetCount = 0;
	m_bOffset = TRUE;
}

void CzerooffsetDlg::OnBnClickedButtonStart()
{
	if (m_bTimer == false)
	{
		m_idEvent = SetMMTimer(50/*2*/); //JW56FR1 sends every 2ms a report, but for better/slower legibility use 50ms
		m_bTimer = true;
		GetDlgItem(IDC_BUTTON_START)->SetWindowTextW(L"Stop");
		GetDlgItem(IDC_BUTTON_OFFSET)->EnableWindow(TRUE);
	}
	else
	{
		KillMMTimer(m_idEvent);
		m_bTimer = false;
		GetDlgItem(IDC_BUTTON_START)->SetWindowTextW(L"Start");
		GetDlgItem(IDC_BUTTON_OFFSET)->EnableWindow(FALSE);
	}
}

void CzerooffsetDlg::OnClose()
{
	KillMMTimer(m_idEvent);
	m_Joywarrior.Close();
	CDialogEx::OnClose();
}


void CzerooffsetDlg::OnBnClickedButtonClear()
{
	m_ListMsg.ResetContent();
}
