// CopyWSRApp.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// CCopyWSRApp:
// See CopyWSRGui.cpp for the implementation of this class
//

class CCopyWSRApp : public CWinApp
{
public:
	CCopyWSRApp();
	void RunService();

protected:
	// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// Implementation

	DECLARE_MESSAGE_MAP()

	BOOL Process();
	CString GetServicePath();

private:
	CString _ServicePath;
};

extern CCopyWSRApp theApp;
