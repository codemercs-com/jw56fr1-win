
// inclinometerDlg.cpp: Implementierungsdatei
//

#include "pch.h"
#include "framework.h"
#include "inclinometer.h"
#include "inclinometerDlg.h"
#include "afxdialogex.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define THREAD_START 1
#define THREAD_STOP 0


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
	DDX_Control(pDX, IDC_SLIDER_ALPHA, m_SliderAlpha);
	DDX_Control(pDX, IDC_STATIC_ALPHA, m_StaticAlpha);
}

BEGIN_MESSAGE_MAP(CinclinometerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CinclinometerDlg::OnBnClickedButtonStart)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CinclinometerDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_FILTER_SET, &CinclinometerDlg::OnBnClickedButtonFilterSet)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ALPHA, &CinclinometerDlg::OnNMCustomdrawSliderAlpha)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CinclinometerDlg-Meldungshandler

BOOL CinclinometerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_SliderAlpha.SetRange(1, 100);
	m_SliderAlpha.SetPos(100);
	m_StaticAlpha.SetWindowTextW(L"0.01");

	m_bTimer = 0;
	m_ThreadData.run = false;
	m_ThreadData.execute = false;
	m_ThreadData.status = THREAD_START;

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
		FilterData temp = { 0,0,0 };

		//Clear offset
		m_OffsetData.accX = 0;
		m_OffsetData.accY = 0;
		m_OffsetData.accZ = 0;

		//Get the offset
		for (int i = 0; i < 255; i++)
		{
			data = m_Joywarrior.GetData();
			m_OffsetData.accX = m_OffsetData.accX + data.accX;
			m_OffsetData.accY = m_OffsetData.accY + data.accY;
			m_OffsetData.accZ = m_OffsetData.accZ + data.accZ;
		}

		m_OffsetData.accX /= 255;
		m_OffsetData.accY /= 255;
		m_OffsetData.accZ /= 255;

		m_OffsetData.accX = m_OffsetData.accX - JW56FR1_ZERO;
		m_OffsetData.accY = m_OffsetData.accY - JW56FR1_ZERO;
		m_OffsetData.accZ = m_OffsetData.accZ - JW56FR1_ZERO + JW56FR1_2G;

		//Clear filter Data
		m_FilterX.Clear();
		m_FilterY.Clear();
		m_FilterZ.Clear();

		//Set ALPHA for down-pass (0.01 to 0.1), 1.0 -> no filter
		m_FilterX.SetAlpha(1.0);
		m_FilterY.SetAlpha(1.0);
		m_FilterZ.SetAlpha(1.0);

		//Start thread for getting data
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

		//Start timer
		SetTimer(IDC_LOOPTIME, 100, FALSE); //As fast as possible
		GetDlgItem(IDC_BUTTON_START)->SetWindowTextW(L"Stop");
		m_bTimer = true;
	}
	else
	{
		KillTimer(IDC_LOOPTIME);
		GetDlgItem(IDC_BUTTON_START)->SetWindowTextW(L"Start");
		m_bTimer = false;
		m_ThreadData.status = THREAD_STOP;
		m_ThreadData.run = false;
	}
}


void CinclinometerDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString text;
	JW56FR1_DATA data;
	int x, y, z = 0;

	//Check for Thread for new data (thread paused)
	m_ThreadData.execute = false;

	//Use filter
	x = m_FilterX.Execute();
	y = m_FilterY.Execute();
	z = m_FilterZ.Execute();

	m_FilterX.Clear();
	m_FilterY.Clear();
	m_FilterZ.Clear();

	//Restart thread
	m_ThreadData.execute = true;

	//Output
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

	yaw = 180 - ((Winkel_Z(x, y, z) * 180 / PI) - 90) * (-1);

	text.Format(L"%0.1f °", pitch);
	m_StaticPitch.SetWindowTextW(text);

	text.Format(L"%0.1f °", roll);
	m_StaticRoll.SetWindowTextW(text);

	text.Format(L"%0.1f °", yaw);
	m_StaticYaw.SetWindowTextW(text);

	CDialogEx::OnTimer(nIDEvent);
}


UINT CinclinometerDlg::Thread_Data(LPVOID pParam)
{
	CinclinometerDlg* pObject = (CinclinometerDlg*)pParam;

	if (pObject == NULL)
		return 1;

	timeBeginPeriod(1); //Set to fastest timer possible
	JW56FR1_DATA data;

	//Get some data to clear
	for (int i = 0; i < 100; i++)
		pObject->m_Joywarrior.GetData();

	//Read jw56fr1 as fast as possible
	for (;;)
	{
		//Close thread external
		if (pObject->m_ThreadData.status == THREAD_STOP)
			break;

		data = pObject->m_Joywarrior.GetData();

		if (pObject->m_ThreadData.execute == false)
			continue;
		else
		{
			pObject->m_FilterX.Add(data.accX - JW56FR1_ZERO - pObject->m_OffsetData.accX);
			pObject->m_FilterY.Add(data.accY - JW56FR1_ZERO - pObject->m_OffsetData.accY);
			pObject->m_FilterZ.Add(data.accZ - JW56FR1_ZERO - pObject->m_OffsetData.accZ);
		}
	}

	pObject->m_ThreadData.run = false;

	return 0;
}

void CinclinometerDlg::OnBnClickedButtonFilterSet()
{
	double alpha = m_SliderAlpha.GetPos() / 100.0;

	/*
	* Alpha = 1.0 : No Filter
	* Less alpha, more filtering
	*/

	m_FilterX.SetAlpha(alpha);
	m_FilterY.SetAlpha(alpha);
	m_FilterZ.SetAlpha(alpha);
}


void CinclinometerDlg::OnNMCustomdrawSliderAlpha(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	CString text;
	int value = m_SliderAlpha.GetPos();
	double alpha = value / 100.0;
	text.Format(L"%0.2f", alpha);

	m_StaticAlpha.SetWindowTextW(text);
	*pResult = 0;
}


void CinclinometerDlg::OnClose()
{
	m_ThreadData.status = THREAD_STOP; //Stop Thread
	KillTimer(IDC_LOOPTIME); //Stop Timer
	m_Joywarrior.Close();

	CDialogEx::OnClose();
}
