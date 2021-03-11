
// rawFileProcessing.h : main header file for the rawFileProcessing application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CrawFileProcessingApp:
// See rawFileProcessing.cpp for the implementation of this class
//

class CrawFileProcessingApp : public CWinApp
{
public:
	CrawFileProcessingApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CrawFileProcessingApp theApp;
