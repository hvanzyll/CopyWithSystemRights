// CLIUsageDlg.cpp : implementation file
//

#include "pch.h"
#include "resource.h"
#include "CLIUsageDlg.h"

// CLIUsageDlg dialog

IMPLEMENT_DYNAMIC(CLIUsageDlg, CDialogEx)

CLIUsageDlg::CLIUsageDlg(CWnd* pParent, BOOL showContinueToGui)
	: CDialogEx(IDD_DIALOG_CLI, pParent)
	, _showContinueToGui(showContinueToGui)
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
	ON_BN_CLICKED(IDC_BUTTON_CONTINUE_GUI, &CLIUsageDlg::OnBnClickedButtonContinueGui)
	ON_BN_CLICKED(IDOK, &CLIUsageDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CLIUsageDlg message handlers

void CLIUsageDlg::OnBnClickedButtonContinueGui()
{
	CDialogEx::OnOK();
}

BOOL CLIUsageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (!_showContinueToGui)
		GetDlgItem(IDC_BUTTON_CONTINUE_GUI)->ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLIUsageDlg::OnBnClickedOk()
{
	_showContinueToGui = FALSE;

	CDialogEx::OnOK();
}