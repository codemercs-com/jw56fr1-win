
// inclinometerDlg.h: Headerdatei
//

#pragma once
#include "CJoyWarrior56FR1.h"
#include "CFilter.h"

#define PI 3.14159265
#define FILTER_LEN 25

// CinclinometerDlg-Dialogfeld
class CinclinometerDlg : public CDialogEx
{
// Konstruktion
public:
	CinclinometerDlg(CWnd* pParent = nullptr);	// Standardkonstruktor

// Dialogfelddaten
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INCLINOMETER_DIALOG };
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

	double Winkel_X(double x, double y, double z);
	double Winkel_Y(double x, double y, double z);
	double Winkel_Z(double x, double y, double z);
	double GetPitch(int x, int y, int z);
	double GetRoll(int x, int y, int z);

	typedef struct OffsetData
	{
		int accX;
		int accY;
		int accZ;
	}OffsetData;

	typedef struct FilterData
	{
		int accX;
		int accY;
		int accZ;
	}FilterData;

	CJoyWarrior56FR1 m_Joywarrior;
	OffsetData m_OffsetData;
	bool m_bTimer;

	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonConnect();
	CStatic m_StaticX;
	CStatic m_StaticY;
	CStatic m_StaticZ;
	CStatic m_StaticRoll;
	CStatic m_StaticPitch;
	CStatic m_StaticYaw;

	typedef struct TheadStruct
	{
		bool run;
		int status;
		bool execute;
	}TheadStruct;

	TheadStruct m_ThreadData;
	static UINT Thread_Data(LPVOID pParam);

	CFilter m_FilterX;
	CFilter m_FilterY;
	CFilter m_FilterZ;

	CSliderCtrl m_SliderAlpha;
	afx_msg void OnBnClickedButtonFilterSet();
	afx_msg void OnNMCustomdrawSliderAlpha(NMHDR* pNMHDR, LRESULT* pResult);
	CStatic m_StaticAlpha;
	afx_msg void OnClose();
};
