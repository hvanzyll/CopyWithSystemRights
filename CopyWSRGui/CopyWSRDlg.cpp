// CopyWSRDlg.cpp : implementation file
//

#include "pch.h"
#include "CopyWSRApp.h"
#include "CopyWSRDlg.h"
#include "CommandWriter.h"
#include "NewFileNameDlg.h"
#include "CLIUsageDlg.h"
#include "AboutDlg.h"

#pragma comment(lib, "Version.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

// CCopyWSRDlg dialog

CCopyWSRDlg::CCopyWSRDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_COPYWSRGUI_DIALOG, pParent)
	, _Directory(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCopyWSRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIRECTORY, _Directory);
	DDX_Control(pDX, IDC_LIST_FILES, _FilesListBox);
}

BEGIN_MESSAGE_MAP(CCopyWSRDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT_DIRECTORY, &CCopyWSRDlg::OnEnChangeEditDirectory)
	ON_BN_CLICKED(IDC_BUTTON_COPY, &CCopyWSRDlg::OnBnClickedButtonCopy)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CCopyWSRDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_RENAME, &CCopyWSRDlg::OnBnClickedButtonRename)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CCopyWSRDlg::OnBnClickedButtonRefresh)
END_MESSAGE_MAP()

// CCopyWSRDlg message handlers

BOOL CCopyWSRDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// add the ability to drag and drop files into the dialog
	ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	CDialog::DragAcceptFiles();

	AddMenuItems();
	UpdateTitleWithVersion();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// create default column for the list control
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_FILES);
	pList->InsertColumn(0, _T("File Name"), LVCFMT_LEFT, 200);

	UpdateFileListBox();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCopyWSRDlg::UpdateTitleWithVersion()
{
	CCopyWSRApp* pApp = (CCopyWSRApp*)AfxGetApp();
	CString version = pApp->GetFileVersionOfApplication();
	if (!version.IsEmpty())
	{
		CString strTitle;
		GetWindowText(strTitle);
		strTitle += _T(" (v:") + version + _T(")");
		SetWindowText(strTitle);
	}
}

void CCopyWSRDlg::AddMenuItems()
{
	// Add "About..." menu item to system menu.

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		CString strCLIMenu;
		BOOL bNameValid = strCLIMenu.LoadString(IDS_CLI_BOX);
		ASSERT(bNameValid);

		ASSERT((IDM_CLI_BOX & 0xFFF0) == IDM_CLI_BOX);
		ASSERT(IDM_CLI_BOX < 0xF000);
		if (!strCLIMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_CLI_BOX, strCLIMenu);
		}

		// IDM_ABOUTBOX must be in the system command range.
		ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
		ASSERT(IDM_ABOUTBOX < 0xF000);
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
}

void CCopyWSRDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	if ((nID & 0xFFF0) == IDM_CLI_BOX)
	{
		CLIUsageDlg dlgUsage(this, false);
		dlgUsage.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCopyWSRDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCopyWSRDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCopyWSRDlg::OnEnChangeEditDirectory()
{
	UpdateFileListBox();
}

void CCopyWSRDlg::UpdateFileListBox()
{
	UpdateData(TRUE);

	// clear the list box
	_FilesListBox.ResetContent();
	if (_Directory.IsEmpty())
		return;

	// get the first file in the directory
	CString strFile = _Directory + _T("\\*.*");
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile(strFile, &findData);
	int i = 0;
	if (hFind != INVALID_HANDLE_VALUE)
	{
		// add the first file to the list control
		AddEntry(findData);

		// get the next file in the directory
		while (FindNextFile(hFind, &findData))
		{
			// check if the file is not a directory
			AddEntry(findData);
		}
		// close the find handle
		FindClose(hFind);
	}
}

void CCopyWSRDlg::AddEntry(WIN32_FIND_DATA& findData)
{
	if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		// add the next file to the list control
		_FilesListBox.AddString(findData.cFileName);
	}
}

void CCopyWSRDlg::OnBnClickedButtonCopy()
{
	// open file dialog brows for all files
	CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, _T("All Files (*.*)|*.*||"));
	if (dlg.DoModal() != IDOK)
		return;

	// get the selected files
	POSITION pos = dlg.GetStartPosition();
	if (pos == NULL)
		return;

	CString filePath = dlg.GetPathName();
	CString fileName = dlg.GetFileName();
	CString destPath = _Directory + _T("\\") + fileName;

	CWaitCursor wait;

	CommandWriter::writeCopyFile(filePath, destPath);

	RunService();
	UpdateFileListBox();
}

void CCopyWSRDlg::OnBnClickedButtonDelete()
{
	// get the selected string
	int index = _FilesListBox.GetCurSel();
	if (index == LB_ERR)
		return;

	CString str;
	_FilesListBox.GetText(index, str);
	// get the full path
	CString fullPath = _Directory + _T("\\") + str;

	CWaitCursor wait;

	CommandWriter::writeDeleteFile(fullPath);

	RunService();
	UpdateFileListBox();
}

void CCopyWSRDlg::OnBnClickedButtonRename()
{
	// get the selected string
	int index = _FilesListBox.GetCurSel();
	if (index == LB_ERR)
		return;

	CString oldFileName;
	_FilesListBox.GetText(index, oldFileName);
	// get the full path
	CString oldFullPath = _Directory + _T("\\") + oldFileName;

	CNewFileNameDlg dlg(this, oldFileName);
	if (dlg.DoModal() != IDOK)
		return;

	// get the new full path
	CString newFullPath = _Directory + _T("\\") + dlg.GetNewFileName();

	CWaitCursor wait;

	CommandWriter::writeRenameFile(oldFullPath, newFullPath);

	RunService();
	UpdateFileListBox();
}

void CCopyWSRDlg::RunService()
{
	CCopyWSRApp* pApp = (CCopyWSRApp*)AfxGetApp();
	pApp->RunService();
}

void CCopyWSRDlg::OnDropFiles(HDROP hDropInfo)
{
	// get the number of files dropped
	UINT nFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	if (nFiles == 1)
	{
		// get the length of the file name
		UINT nLength = DragQueryFile(hDropInfo, 0, NULL, 0);
		// get the file name
		TCHAR* szFileName = new TCHAR[nLength + 1];
		DragQueryFile(hDropInfo, 0, szFileName, nLength + 1);
		// get the file name
		CString sourceFilePathName = szFileName;
		delete[] szFileName;

		// get the file path
		CString path;
		int index = sourceFilePathName.ReverseFind('\\');
		if (index != -1)
		{
			CString fileName = sourceFilePathName.Mid(index + 1);
			CString destPathName = _Directory + _T("\\") + fileName;

			CWaitCursor wait;

			CommandWriter::writeCopyFile(sourceFilePathName, destPathName);

			RunService();
			UpdateFileListBox();
		}
	}

	CDialog::OnDropFiles(hDropInfo);
}

void CCopyWSRDlg::OnBnClickedButtonRefresh()
{
	UpdateFileListBox();
}