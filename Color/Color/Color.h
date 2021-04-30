
// Color.h : main header file for the Color application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CColorApp:
// See Color.cpp for the implementation of this class
//

class CColorApp : public CWinApp
{
public:
	CColorApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CColorApp theApp;
