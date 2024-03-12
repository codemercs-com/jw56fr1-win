
// ShowData.h: Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'stdafx.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole


// CShowDataApp:
// Siehe ShowData.cpp für die Implementierung dieser Klasse
//

class CShowDataApp : public CWinApp
{
public:
	CShowDataApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CShowDataApp theApp;