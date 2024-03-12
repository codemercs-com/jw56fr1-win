
// speedtestDlg.h: Headerdatei
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <winver.h>


// CspeedtestDlg-Dialogfeld
class CspeedtestDlg : public CDialogEx
{
// Konstruktion
public:
	CspeedtestDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPEEDTEST_DIALOG };
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

	UINT m_nIDEvent;

	static void CALLBACK MM_TimerFunction(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);
	UINT MM_SetTimer(UINT ms);
	void MM_KillTimer(UINT nIDEvent);
	void MM_TimerHandler(UINT nIDEvent);
	bool m_bRun;


	JW_HANDLES m_Device;
	CString m_SerialList[JW_MAX_DEVICES];
	HIDP_CAPS m_Caps;

	CFile m_File;
	CString m_Path;
	bool m_IsFile;

	void SetConfig();
	void ChangeBwInput(int type);


	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	CComboBox m_DeviceList;
	CComboBox m_Mode;
	int m_ModeType;
	afx_msg void OnCbnSelchangeCmbMode();
	afx_msg void OnCbnSelchangeCombo1();
	CProgressCtrl m_Progress;
	int m_ProgCount;
	CComboBox m_ARange;
	CComboBox m_AFilter;
	CComboBox m_ABand;
	CComboBox m_ACut;
	CComboBox m_RCut;
	CComboBox m_RRange;
	CButton m_RHp;
	afx_msg void OnCbnSelchangeCombo4();
	CString GetVersion();
};
