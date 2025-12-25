// CopyWSRDlg.h : header file
//

#pragma once

// CCopyWSRDlg dialog
class CCopyWSRDlg : public CDialogEx
{
	enum SortBy { FILENAME_LOW, FILENAME_HIGH, SIZE_LOW, SIZE_HIGH, DATE_LOW, DATE_HIGH };

	// Construction
public:
	CCopyWSRDlg(CWnd* pParent = nullptr);	// standard constructor

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COPYWSRGUI_DIALOG };
#endif

protected:

	virtual BOOL OnInitDialog();
	void UpdateTitleWithVersion();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
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
	afx_msg void OnDropFiles(HDROP);
	afx_msg void OnListFilesColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonBackup();
	afx_msg void OnBnClickedButtonRestore();
	afx_msg void OnLvnItemchangedListFiles(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonBrowse();
	DECLARE_MESSAGE_MAP()

	void Sort();

	void AddMenuItems();
	void AddEntryToList(WIN32_FIND_DATA& findData);
	void UpdateFileListBox();
	void AddFilesToListControl();
	void AddEntry(WIN32_FIND_DATA& findData);
	void RunService();
	BOOL IsBackupFile(CString str);
	CSize _UIMinSize;

private:
	// Variables
	HICON m_hIcon;
	CString _Directory;
	CListCtrl _FilesListBox;
	std::list< WIN32_FIND_DATA> _FileListData;
	CButton _RestoreBtn;
	SortBy _CurrentSort;
public:
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
};