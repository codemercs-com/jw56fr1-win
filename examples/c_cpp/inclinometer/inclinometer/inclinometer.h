
// inclinometer.h: Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'pch.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole


// CinclinometerApp:
// Siehe inclinometer.cpp für die Implementierung dieser Klasse
//

class CinclinometerApp : public CWinApp
{
public:
	CinclinometerApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CinclinometerApp theApp;
