// CopyWSRGui.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "CopyWSRApp.h"
#include "CopyWSRDlg.h"
#include "CommandWriter.h"

#include "..\ServiceLib\ServiceControl.h"
#include "CLIUsageDlg.h"
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
		dlg.DoModal();
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
	BOOL needsRunService = FALSE;
	BOOL runUI = TRUE;
	if (argc > 0 && argv != NULL)
	{
		// check if command is copy
		if (_wcsicmp(argv[0], L"/copy") == 0 && argc == 3)
		{
			// copy the file
			CommandWriter::writeCopyFile(argv[1], argv[2]);
			needsRunService = TRUE;
		}
		else if (_wcsicmp(argv[0], L"/delete") == 0 && argc == 2)
		{
			// copy the file
			CommandWriter::writeDeleteFile(argv[1]);
			needsRunService = TRUE;
		}
		else if (_wcsicmp(argv[0], L"/rename") == 0 && argc == 3)
		{
			// copy the file
			CommandWriter::writeRenameFile(argv[1], argv[2]);
			needsRunService = TRUE;
		}
		else if (m_lpCmdLine[0] != 0)
		{
			CLIUsageDlg dlgUsage(nullptr, true);
			dlgUsage.DoModal();

			runUI = dlgUsage.IsShowContinueToGui();
		}
	}

	if (needsRunService)
	{
		RunService();
		runUI = FALSE;
	}

	return runUI;
}

BOOL CCopyWSRApp::RunService()
{
	CommandWriter::deleteResponseFile();

	if (!ServiceFileExists())
		ExtractService();

	if (!ServiceControl::isServiceRegistered(L"CopyWSRSvc"))
	{
		TCHAR path[MAX_PATH];
		CommandWriter::getServiceFilePath(path, MAX_PATH);

		UINT result = ServiceControl::registerService(path, L"CopyWSRSvc", L"CopyWSRSvc");
		if (result != ERROR_SUCCESS)
		{
			CString s;
			s.Format(_T("registerService error with 0x%X"), result);
			AfxMessageBox(s);
			return FALSE;
		}
	}

	UINT result = ServiceControl::startService(L"CopyWSRSvc");
	if (result != ERROR_SUCCESS)
	{
		CString s;
		s.Format(_T("startService error with 0x%X"), result);
		AfxMessageBox(s);
		return FALSE;
	}

	TCHAR resposeData[4096] = { 0 };
	int counter = 0;
	while (!CommandWriter::readResposeFile(resposeData, MAX_PATH) && counter < 100)
		Sleep(10);

	if (resposeData[0] == 0)
	{
		AfxMessageBox(L"Timeout waiting for response", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	// if first character is 0 then return true
	if (resposeData[0] == '0')
		return TRUE;

	// show error message
	AfxMessageBox(resposeData, MB_OK | MB_ICONEXCLAMATION);
	return FALSE;
}

BOOL CCopyWSRApp::ServiceFileExists()
{
	TCHAR path[MAX_PATH];
	CommandWriter::getServiceFilePath(path, MAX_PATH);
	CString svcPath = path;

	if (0xffffffff == GetFileAttributes(svcPath))
	{
		// file not found
		return FALSE;
	}

	return TRUE;
}

void CCopyWSRApp::ExtractService()
{
	TCHAR path[MAX_PATH];
	CommandWriter::getServiceFilePath(path, MAX_PATH);

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
				HANDLE hFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
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
}

int CCopyWSRApp::ExitInstance()
{
	TCHAR path[MAX_PATH];
	CommandWriter::getServiceFilePath(path, MAX_PATH);
	CString svcPath = path;
	ServiceControl::stopService(L"CopyWSRSvc");
	ServiceControl::unregisterService(L"CopyWSRSvc");
	DeleteFile(svcPath);

	CommandWriter::deleteCommandFile();
	CommandWriter::deleteResponseFile();
	CommandWriter::deleteCommandDir();

	return CWinApp::ExitInstance();
}

CString CCopyWSRApp::GetFileVersionOfApplication()
{
	TCHAR szFileName[MAX_PATH] = { 0 };
	DWORD dwLen = GetModuleFileName(NULL, szFileName, MAX_PATH);
	if (dwLen == 0)
		return _T("");

	DWORD dwDummy = 0;
	DWORD dwFVISize = GetFileVersionInfoSize(szFileName, &dwDummy);

	LPBYTE lpVersionInfo = new BYTE[dwFVISize];

	GetFileVersionInfo(szFileName, 0, dwFVISize, lpVersionInfo);

	UINT uLen = 0;
	VS_FIXEDFILEINFO* lpFfi = nullptr;
	VerQueryValue(lpVersionInfo, _T("\\"), (LPVOID*)&lpFfi, &uLen);

	DWORD dwFileVersionMS = lpFfi->dwFileVersionMS;
	DWORD dwFileVersionLS = lpFfi->dwFileVersionLS;

	delete[] lpVersionInfo;

	DWORD dwLeftMost = HIWORD(dwFileVersionMS);
	DWORD dwSecondLeft = LOWORD(dwFileVersionMS);
	DWORD dwSecondRight = HIWORD(dwFileVersionLS);
	DWORD dwRightMost = LOWORD(dwFileVersionLS);

	CString str;
	str.Format(L"%d.%d.%d.%d\n", dwLeftMost, dwSecondLeft, dwSecondRight, dwRightMost);

	return str;
}