// CLIUsageDlg.cpp : implementation file
//

#include "pch.h"
#include "resource.h"
#include "CLIUsageDlg.h"

// CLIUsageDlg dialog

IMPLEMENT_DYNAMIC(CLIUsageDlg, CDialogEx)

CLIUsageDlg::CLIUsageDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CLI, pParent)
{
}

CLIUsageDlg::~CLIUsageDlg()
{
}

void CLIUsageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLIUsageDlg, CDialogEx)
END_MESSAGE_MAP()

// CLIUsageDlg message handlers