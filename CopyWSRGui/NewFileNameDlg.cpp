// NewFileNameDlg.cpp : implementation file
//

#include "pch.h"
#include "resource.h"
#include "NewFileNameDlg.h"

// CNewFileNameDlg dialog

IMPLEMENT_DYNAMIC(CNewFileNameDlg, CDialogEx)

CNewFileNameDlg::CNewFileNameDlg(CWnd* pParent, CString oldFileName)
	: CDialogEx(IDD_DIALOG_NEW_FILE_NAME, pParent)
	, _NewFileName(oldFileName)
{
}

CNewFileNameDlg::~CNewFileNameDlg()
{
}

void CNewFileNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NEW_FILE_NAME, _NewFileName);
}

BEGIN_MESSAGE_MAP(CNewFileNameDlg, CDialogEx)
END_MESSAGE_MAP()

// CNewFileNameDlg message handlers