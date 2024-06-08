
// zero_offsetDlg.h: Headerdatei
//

#pragma once
#include "CJoyWarrior56FR1.h"


// CzerooffsetDlg-Dialogfeld
class CzerooffsetDlg : public CDialogEx
{
// Konstruktion
public:
	CzerooffsetDlg(CWnd* pParent = nullptr);	// Standardkonstruktor

// Dialogfelddaten
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ZERO_OFFSET_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CJoyWarrior56FR1 m_Joywarrior;

	typedef struct OffsetData
	{
		int accX;
		int accY;
		int accZ;
	}OffsetData;

	BOOL m_bOffset;
	int m_OffsetCount;
	OffsetData m_OffsetData;

	static void CALLBACK TimerFunction(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);
	BOOL m_bTimer;
	UINT m_idEvent;
	void MMTimerHandler(UINT nIDEvent);
	UINT SetMMTimer(UINT ms);
	void KillMMTimer(UINT nIDEvent);

	CStatic m_StaticX1;
	CStatic m_StaticY1;
	CStatic m_StaticZ1;
	CStatic m_StaticX2;
	CStatic m_StaticY2;
	CStatic m_StaticZ2;
	afx_msg void OnBnClickedButtonOffset();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonClear();
	CListBox m_ListMsg;
};
