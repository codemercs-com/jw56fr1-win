
// speedtest.h: Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'stdafx.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole


// CspeedtestApp:
// Siehe speedtest.cpp für die Implementierung dieser Klasse
//

class CspeedtestApp : public CWinApp
{
public:
	CspeedtestApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CspeedtestApp theApp;