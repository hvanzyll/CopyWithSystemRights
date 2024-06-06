#pragma once

// CLIUsageDlg dialog

class CLIUsageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLIUsageDlg)

public:
	CLIUsageDlg(CWnd* pParent, BOOL showContinueToGui);   // standard constructor
	virtual ~CLIUsageDlg();

	BOOL IsShowContinueToGui() const { return _showContinueToGui; }

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CLI };
#endif

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonContinueGui();

private:
	BOOL _showContinueToGui;
public:
	afx_msg void OnBnClickedOk();
};
