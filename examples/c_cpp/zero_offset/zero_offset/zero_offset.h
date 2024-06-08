
// zero_offset.h: Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'pch.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole


// CzerooffsetApp:
// Siehe zero_offset.cpp für die Implementierung dieser Klasse
//

class CzerooffsetApp : public CWinApp
{
public:
	CzerooffsetApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CzerooffsetApp theApp;
