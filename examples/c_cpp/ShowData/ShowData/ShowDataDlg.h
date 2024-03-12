
// ShowDataDlg.h: Headerdatei
//

#pragma once
#include "afxwin.h"



// CShowDataDlg-Dialogfeld
class CShowDataDlg : public CDialogEx
{
// Konstruktion
public:
	CShowDataDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHOWDATA_DIALOG };
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

	struct ConversionData
	{
		double acc;
		double rot;
	};

	ConversionData m_Conversion;

	UINT m_nIDEvent;

	static void CALLBACK MM_TimerFunction(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);
	UINT MM_SetTimer(UINT ms);
	void MM_KillTimer(UINT nIDEvent);
	void MM_TimerHandler(UINT nIDEvent);

	afx_msg void OnBnClickedBtnSearch();
	afx_msg void OnBnClickedBtnRun();

	bool m_bRun;

	CComboBox m_DeviceList;
	CString m_SerialList[JW_MAX_DEVICES];
	JW_HANDLES m_Device;
	CComboBox m_ARange;
	CComboBox m_ACutoff;
	CComboBox m_ABand;
	CComboBox m_GCutoff;
	CComboBox m_GRange;
	afx_msg void OnBnClickedBtnSettings();
	CButton m_GHp;
	CComboBox m_Mode;
	afx_msg void OnCbnSelchangeCmbDevice();
	CComboBox m_AFilter;
	void ChangeBwInput(int type);
	afx_msg void OnCbnSelchangeCmbSettingAfilter();

	

	afx_msg void OnBnClickedBtnStore();
	void WriteConfig();
};
