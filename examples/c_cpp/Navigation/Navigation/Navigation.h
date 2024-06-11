
// Navigation.h: Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'pch.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole


// CNavigationApp:
// Siehe Navigation.cpp für die Implementierung dieser Klasse
//

class CNavigationApp : public CWinApp
{
public:
	CNavigationApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CNavigationApp theApp;
