// CopyWSRGui.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "CopyWSRApp.h"
#include "CopyWSRDlg.h"
#include "CommandWriter.h"

#include "..\ServiceLib\ServiceControl.h"
#pragma comment(lib, "SimpleServiceLib.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCopyWSRApp

BEGIN_MESSAGE_MAP(CCopyWSRApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CCopyWSRApp construction

CCopyWSRApp::CCopyWSRApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CCopyWSRApp object

CCopyWSRApp theApp;

// CCopyWSRApp initialization

BOOL CCopyWSRApp::InitInstance()
{
	// make sure this is the only instance of the application
	HANDLE hMutex = CreateMutex(NULL, TRUE, L"CopyWSR");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		MessageBox(NULL, L"Another instance of the application is already running.", L"Copy With System Rights", MB_OK);
		// set focus to existing instance
		HWND hWnd = FindWindow(NULL, L"Copy With System Rights");
		if (hWnd != NULL)
		{
			SetForegroundWindow(hWnd);
		}
		return FALSE;
	}

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager* pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// get the command line arguments

	if (Process())
	{
		CCopyWSRDlg dlg;
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
		}
		else if (nResponse == -1)
		{
			TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
			TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
		}
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CCopyWSRApp::Process()
{
	int argc;
	LPWSTR* argv = CommandLineToArgvW(m_lpCmdLine, &argc);
	BOOL needsProcess = TRUE;
	if (argc > 1 && argv != NULL)
	{
		// check if command is copy
		if (_wcsicmp(argv[0], L"/copy") == 0 && argc == 3)
		{
			// copy the file
			CommandWriter::writeCopyFile(argv[1], argv[2]);
			needsProcess = false;
		}
		else if (_wcsicmp(argv[0], L"/delete") == 0 && argc == 2)
		{
			// copy the file
			CommandWriter::writeDeleteFile(argv[1]);
			needsProcess = false;
		}
		else if (_wcsicmp(argv[0], L"/rename") == 0 && argc == 3)
		{
			// copy the file
			CommandWriter::writeRenameFile(argv[1], argv[2]);
			needsProcess = false;
		}
		else
		{
			CString out = L"Usage: CopyWSR.exe[/copy | /delete | /rename][source][destination]\n";
			MessageBox(NULL, out, L"Copy With System Rights", MB_OK);
		}
	}

	if (!needsProcess)
		RunService();

	return needsProcess;
}

void CCopyWSRApp::RunService()
{
	if (!ServiceControl::isServiceRegistered(L"CopyWSRSvc"))
	{
		CString servicePath = GetServicePath();
		ServiceControl::registerService(servicePath, L"CopyWSRSvc", L"Copy With System Rights, Service");
	}

	ServiceControl::startService(L"CopyWSRSvc");
	Sleep(1000);
}

CString CCopyWSRApp::GetServicePath()
{
	ASSERT(_ServicePath.IsEmpty());
	TCHAR path[MAX_PATH];
	CommandWriter::getServiceFilePath(path, MAX_PATH);
	_ServicePath = path;

	// extract IDR_COPY_WRC_SVC_EXE from the resource
	HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(IDR_COPY_WRC_SVC_EXE), L"RC_DATA");
	if (hRes != NULL)
	{
		HGLOBAL hResData = LoadResource(NULL, hRes);
		if (hResData != NULL)
		{
			LPVOID pResData = LockResource(hResData);
			if (pResData != NULL)
			{
				HANDLE hFile = CreateFile(_ServicePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					DWORD dwSize = SizeofResource(NULL, hRes);
					DWORD dwWritten;
					WriteFile(hFile, pResData, dwSize, &dwWritten, NULL);
					CloseHandle(hFile);
				}
			}
		}
	}

	return _ServicePath;
}

int CCopyWSRApp::ExitInstance()
{
	if (!_ServicePath.IsEmpty())
	{
		ServiceControl::stopService(L"CopyWSRSvc");
		ServiceControl::unregisterService(L"CopyWSRSvc");
		DeleteFile(_ServicePath);
		_ServicePath.Empty();
	}

	CommandWriter::deleteCommandFile();

	return CWinApp::ExitInstance();
}