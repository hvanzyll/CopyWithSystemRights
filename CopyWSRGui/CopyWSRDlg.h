// CopyWSRDlg.h : header file
//

#pragma once

// CCopyWSRDlg dialog
class CCopyWSRDlg : public CDialogEx
{
	// Construction
public:
	CCopyWSRDlg(CWnd* pParent = nullptr);	// standard constructor

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COPYWSRGUI_DIALOG };
#endif

protected:

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void OnOK();
	virtual BOOL DestroyWindow();

	// Generated message map functions
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEnChangeEditDirectory();
	afx_msg void OnBnClickedButtonCopy();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonRename();
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnDropFiles(HDROP);
	DECLARE_MESSAGE_MAP()

	void UpdateTitleWithVersion();
	void AddMenuItems();
	void UpdateFileListBox();
	void AddEntry(WIN32_FIND_DATA& findData);
	void RunService();

private:
	// Variables
	HICON m_hIcon;
	CString _Directory;
	CListBox _FilesListBox;
};
