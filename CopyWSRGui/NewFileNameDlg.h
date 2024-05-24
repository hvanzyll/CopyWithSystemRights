#pragma once
#include "afxdialogex.h"

// CNewFileNameDlg dialog

class CNewFileNameDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNewFileNameDlg)

public:
	CNewFileNameDlg(CWnd* pParent, CString oldFileName);   // standard constructor
	virtual ~CNewFileNameDlg();

	const CString GetNewFileName() { return _NewFileName; }

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_NEW_FILE_NAME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CString _NewFileName;
};
