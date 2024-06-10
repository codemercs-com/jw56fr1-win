
// inclinometerDlg.cpp: Implementierungsdatei
//

#include "pch.h"
#include "framework.h"
#include "inclinometer.h"
#include "inclinometerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CinclinometerDlg::CinclinometerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INCLINOMETER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CinclinometerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_X, m_StaticX);
	DDX_Control(pDX, IDC_STATIC_Y, m_StaticY);
	DDX_Control(pDX, IDC_STATIC_Z, m_StaticZ);
	DDX_Control(pDX, IDC_STATIC_ROLL, m_StaticRoll);
	DDX_Control(pDX, IDC_STATIC_PITCH, m_StaticPitch);
	DDX_Control(pDX, IDC_STATIC_YAW, m_StaticYaw);
}

BEGIN_MESSAGE_MAP(CinclinometerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CinclinometerDlg::OnBnClickedButtonStart)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CinclinometerDlg::OnBnClickedButtonConnect)
END_MESSAGE_MAP()


// CinclinometerDlg-Meldungshandler

BOOL CinclinometerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);


	m_bTimer = 0;

	return TRUE;
}

void CinclinometerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

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

HCURSOR CinclinometerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CinclinometerDlg::OnBnClickedButtonConnect()
{
	int num = m_Joywarrior.Open();

	if (num > 0)
	{
		m_Joywarrior.SetMode(JW56FR1_MODE_NORMAL);
		m_Joywarrior.SetParameter(JW56FR1_ARNG_RANGE_2G, JW56FR1_AFILT_NONE, JW56FR1_GRNG_245DPS, JW56FR1_GFILT_NONE); //Start-up values from JW56FR1

		GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	}
	else
	{
		MessageBox(L"No JoyWarrior56FR1 found on USB", L"Device Error", MB_ICONERROR | MB_OK);
	}
}


double CinclinometerDlg::Winkel_X(double x, double y, double z)
{
	double tf = sqrt((x * x) + (y * y) + (z * z));
	return asin(x / tf);
}

double CinclinometerDlg::Winkel_Y(double x, double y, double z)
{
	double tf = sqrt((x * x) + (y * y) + (z * z));
	return asin(y / tf);
}

double CinclinometerDlg::Winkel_Z(double x, double y, double z)
{
	double tf = sqrt((x * x) + (y * y) + (z * z));
	return asin(z / tf);
}

double CinclinometerDlg::GetPitch(int x, int y, int z)
{
	double erg = 0.0;
	erg = y / sqrtl((x * x) + (z * z));
	erg = atan(erg);

	return erg * 180 / PI;
}

double CinclinometerDlg::GetRoll(int x, int y, int z)
{
	double erg = 0.0;
	erg = x / sqrtl((y * y) + (z * z));
	erg = atan(erg);

	return erg * 180 / PI;
}



void CinclinometerDlg::OnBnClickedButtonStart()
{
	CString text;

	if (m_bTimer == false)
	{
		//Get offset	
		JW56FR1_DATA data;

		//Clear offset
		m_OffsetData.accX = 0;
		m_OffsetData.accY = 0;
		m_OffsetData.accZ = 0;

		for (int i = 0; i < 255; i++)
		{
			data = m_Joywarrior.GetData();
			m_OffsetData.accX = (m_OffsetData.accX + data.accX) / 2;
			m_OffsetData.accY = (m_OffsetData.accY + data.accY) / 2;
			m_OffsetData.accZ = (m_OffsetData.accZ + data.accZ) / 2;
		}

		m_OffsetData.accX = m_OffsetData.accX - JW56FR1_ZERO;
		m_OffsetData.accY = m_OffsetData.accY - JW56FR1_ZERO;
		m_OffsetData.accZ = m_OffsetData.accZ - JW56FR1_ZERO + JW56FR1_2G;

		//Start timer
		SetTimer(IDC_LOOPTIME, 100, FALSE);
		GetDlgItem(IDC_BUTTON_START)->SetWindowTextW(L"Stop");
		m_bTimer = true;
	}
	else
	{
		KillTimer(IDC_LOOPTIME);
		GetDlgItem(IDC_BUTTON_START)->SetWindowTextW(L"Start");
		m_bTimer = false;
	}
}


void CinclinometerDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString text;
	JW56FR1_DATA data;
	data = m_Joywarrior.GetData();
	int x, y, z = 0;

	x = data.accX - JW56FR1_ZERO - m_OffsetData.accX;
	y = data.accY - JW56FR1_ZERO - m_OffsetData.accY;
	z = data.accZ - JW56FR1_ZERO - m_OffsetData.accZ;

	text.Format(L"%d", x);
	m_StaticX.SetWindowTextW(text);

	text.Format(L"%d", y);
	m_StaticY.SetWindowTextW(text);

	text.Format(L"%d", z);
	m_StaticZ.SetWindowTextW(text);

	double roll, pitch, yaw = 0.0;

	pitch = Winkel_X(x, y, z) * 180 / PI;
	roll = Winkel_Y(x, y, z) * 180 / PI;
	//pitch = GetPitch(x, y, z);
	//roll = GetRoll(x, y, z);

	yaw = 180 - ((Winkel_Z(x, y, z) * 180 / PI) - 90)* (-1);


	text.Format(L"%0.1f °", pitch);
	m_StaticPitch.SetWindowTextW(text);

	text.Format(L"%0.1f °", roll);
	m_StaticRoll.SetWindowTextW(text);

	text.Format(L"%0.1f °", yaw);
	m_StaticYaw.SetWindowTextW(text);

	CDialogEx::OnTimer(nIDEvent);
}