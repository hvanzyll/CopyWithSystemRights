#pragma once

// CLIUsageDlg dialog

class CLIUsageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLIUsageDlg)

public:
	CLIUsageDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLIUsageDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CLI };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
