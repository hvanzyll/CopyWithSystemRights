#include "pch.h"
#include "resource.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "CopyWSRApp.h"

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// get the version of the application
	CCopyWSRApp* pApp = (CCopyWSRApp*)AfxGetApp();
	CString version = pApp->GetFileVersionOfApplication();

	CString nameVersion = L"Copy With System Rights, Ver:" + version;

	SetDlgItemText(IDC_STATIC_NAME_VERSION, nameVersion);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()