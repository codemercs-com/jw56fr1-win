
// speedtest.h: Hauptheaderdatei f�r die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'stdafx.h' vor dieser Datei f�r PCH einschlie�en"
#endif

#include "resource.h"		// Hauptsymbole


// CspeedtestApp:
// Siehe speedtest.cpp f�r die Implementierung dieser Klasse
//

class CspeedtestApp : public CWinApp
{
public:
	CspeedtestApp();

// �berschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CspeedtestApp theApp;