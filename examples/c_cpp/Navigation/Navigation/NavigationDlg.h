
// NavigationDlg.h: Headerdatei
//

#pragma once
#include "CJoyWarrior56FR1.h"


// CNavigationDlg-Dialogfeld
class CNavigationDlg : public CDialogEx
{
// Konstruktion
public:
	CNavigationDlg(CWnd* pParent = nullptr);	// Standardkonstruktor

// Dialogfelddaten
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NAVIGATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonRun();
	afx_msg void OnBnClickedButtonReset();

	struct StaticGroup
	{
		CStatic x;
		CStatic y;
		CStatic z;
	};


	StaticGroup m_StaticRawAcc;
	StaticGroup m_StaticAcceleration;
	StaticGroup m_StaticComp; 
	StaticGroup m_StaticSpeed;
	StaticGroup m_StaticTravel;

	CJoyWarrior56FR1 m_Joywarrior;
	afx_msg void OnClose();


	void PumpMsgQueue();

	static void CALLBACK TimerFunction(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);
	UINT m_idEvent;
	void MMTimerHandler(UINT nIDEvent);
	UINT SetMMTimer(UINT ms);
	void KillMMTimer(UINT nIDEvent);
	BOOL m_bTimer;


	typedef struct SpeedStruct
	{
		double x;
		double y;
		double z;

	}SpeedStruct;

	typedef struct SensorDataZero
	{
		int x;
		int y;
		int z;
	} SensorDataZero;

	SensorDataZero m_Offset;
	SpeedStruct m_Acceleration;
	SpeedStruct m_Speed;
	SpeedStruct m_Travel;
	afx_msg void OnBnClickedButtonInit();

	CComboBox m_ABand;
	CComboBox m_AFilter;
	CComboBox m_ACutOff;
	afx_msg void OnBnClickedButtonAfilter();
	afx_msg void OnCbnSelchangeCmbAccFilter();
	void ChangeBwInput(int type);
};
