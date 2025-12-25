// CopyWSRDlg.cpp : implementation file
//

#include "pch.h"
#include "CopyWSRApp.h"
#include "CopyWSRDlg.h"
#include "CommandWriter.h"
#include "NewFileNameDlg.h"
#include "CLIUsageDlg.h"
#include "AboutDlg.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define FILENAME_COLUMN_INDEX 0
#define SIZE_COLUMN_INDEX 1
#define DATE_COLUMN_INDEX 2
#define BACKUP_FILE_EXTENSION _T(".wsrbackup")

// CAboutDlg dialog used for App About

// CCopyWSRDlg dialog

CCopyWSRDlg::CCopyWSRDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_COPYWSRGUI_DIALOG, pParent)
	, _Directory(_T(""))
	, _CurrentSort(FILENAME_LOW)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCopyWSRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DIRECTORY, _Directory);
	DDX_Control(pDX, IDC_LIST_FILES, _FilesListBox);
	DDX_Control(pDX, IDC_BUTTON_RESTORE, _RestoreBtn);
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
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_FILES, &CCopyWSRDlg::OnListFilesColumnClick)
	ON_BN_CLICKED(IDC_BUTTON_BACKUP, &CCopyWSRDlg::OnBnClickedButtonBackup)
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, &CCopyWSRDlg::OnBnClickedButtonRestore)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FILES, &CCopyWSRDlg::OnLvnItemchangedListFiles)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CCopyWSRDlg::OnBnClickedButtonBrowse)
	ON_WM_SIZING()
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

	CSettings settings;
	if (settings.Load())
	{
		_Directory = settings.GetPath();
		UpdateData(FALSE);
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	_RestoreBtn.EnableWindow(FALSE);

	// create default column for the list control
	CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_FILES);
	pList->InsertColumn(FILENAME_COLUMN_INDEX, _T("File Name"), LVCFMT_LEFT, 350);
	pList->InsertColumn(SIZE_COLUMN_INDEX, _T("Size"), LVCFMT_RIGHT, 100);
	pList->InsertColumn(DATE_COLUMN_INDEX, _T("Date/Time"), LVCFMT_RIGHT, 200);

	UpdateFileListBox();

	CRect rect;
	GetWindowRect(&rect);
	_UIMinSize.cx = rect.Width();
	_UIMinSize.cy = rect.Height();

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

void CCopyWSRDlg::OnListFilesColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int nColClicked = pNMListView->iSubItem;
	switch (nColClicked)
	{
	case FILENAME_COLUMN_INDEX:
		if (_CurrentSort == FILENAME_LOW)
			_CurrentSort = FILENAME_HIGH;
		else
			_CurrentSort = FILENAME_LOW;
		break;

	case SIZE_COLUMN_INDEX:
		if (_CurrentSort == SIZE_LOW)
			_CurrentSort = SIZE_HIGH;
		else
			_CurrentSort = SIZE_LOW;
		break;
	case DATE_COLUMN_INDEX:
		if (_CurrentSort == DATE_LOW)
			_CurrentSort = DATE_HIGH;
		else
			_CurrentSort = DATE_LOW;
		break;
	}

	AddFilesToListControl();
}

void CCopyWSRDlg::Sort()
{
	switch (_CurrentSort)
	{
	case FILENAME_LOW:

		_FileListData.sort(
			[](const WIN32_FIND_DATA& a, const WIN32_FIND_DATA& b) {
				return std::wstring(a.cFileName) < std::wstring(b.cFileName);
			}
		);
		break;
	case FILENAME_HIGH:

		_FileListData.sort(
			[](const WIN32_FIND_DATA& a, const WIN32_FIND_DATA& b) {
				return std::wstring(a.cFileName) > std::wstring(b.cFileName);
			}
		);
		break;
	case SIZE_LOW:
		_FileListData.sort(
			[](const WIN32_FIND_DATA& a, const WIN32_FIND_DATA& b) {
				ULARGE_INTEGER sizeA, sizeB;
				sizeA.LowPart = a.nFileSizeLow;
				sizeA.HighPart = a.nFileSizeHigh;
				sizeB.LowPart = b.nFileSizeLow;
				sizeB.HighPart = b.nFileSizeHigh;
				return sizeA.QuadPart < sizeB.QuadPart;
			}
		);
		break;
	case SIZE_HIGH:
		_FileListData.sort(
			[](const WIN32_FIND_DATA& a, const WIN32_FIND_DATA& b) {
				ULARGE_INTEGER sizeA, sizeB;
				sizeA.LowPart = a.nFileSizeLow;
				sizeA.HighPart = a.nFileSizeHigh;
				sizeB.LowPart = b.nFileSizeLow;
				sizeB.HighPart = b.nFileSizeHigh;
				return sizeA.QuadPart > sizeB.QuadPart;
			}
		);
	case DATE_LOW:
		_FileListData.sort(
			[](const WIN32_FIND_DATA& a, const WIN32_FIND_DATA& b) {
				FILETIME ftA = a.ftLastWriteTime;
				FILETIME ftB = b.ftLastWriteTime;
				return CompareFileTime(&ftA, &ftB) < 0;
			}
		);
		break;
	case DATE_HIGH:
		_FileListData.sort(
			[](const WIN32_FIND_DATA& a, const WIN32_FIND_DATA& b) {
				FILETIME ftA = a.ftLastWriteTime;
				FILETIME ftB = b.ftLastWriteTime;
				return CompareFileTime(&ftA, &ftB) > 0;
			}
		);

		break;
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

void CCopyWSRDlg::AddEntryToList(WIN32_FIND_DATA& findData)
{
	_FileListData.push_back(findData);
}

void CCopyWSRDlg::UpdateFileListBox()
{
	UpdateData(TRUE);

	// clear the list box

	_FileListData.clear();
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
		AddEntryToList(findData);

		// get the next file in the directory
		while (FindNextFile(hFind, &findData))
		{
			// check if the file is not a directory
			AddEntryToList(findData);
		}
		// close the find handle
		FindClose(hFind);
	}

	// add all files to the list control
	AddFilesToListControl();
}

void CCopyWSRDlg::AddFilesToListControl()
{
	_FilesListBox.DeleteAllItems();
	Sort();
	for (auto& entry : _FileListData)
	{
		AddEntry(entry);
	}
}

static int AddData(CListCtrl& ctrl, int row, int col, WCHAR* str)
{
	LVITEM lv;
	lv.iItem = row;
	lv.iSubItem = col;
	lv.pszText = str;
	lv.mask = LVIF_TEXT;
	if (col == 0)
		return ctrl.InsertItem(&lv);
	else
		return ctrl.SetItem(&lv);
}

static std::wstring FormatNumber(ULONGLONG number)
{
	const wchar_t* suffixes[] = { L"", L"K", L"M", L"G", L"T", L"P", L"E" };
	int suffixIndex = 0;
	double displayNumber = static_cast<double>(number);
	while (displayNumber >= 1024.0 && suffixIndex < _countof(suffixes) - 1)
	{
		displayNumber /= 1024.0;
		suffixIndex++;
	}
	wchar_t buffer[64];
	swprintf_s(buffer, L"%.2f %sB", displayNumber, suffixes[suffixIndex]);
	return std::wstring(buffer);
}

void CCopyWSRDlg::AddEntry(WIN32_FIND_DATA& findData)
{
	if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		int count = _FilesListBox.GetItemCount();
		int index = AddData(_FilesListBox, count, FILENAME_COLUMN_INDEX, findData.cFileName);
		if (index != LB_ERR && index != LB_ERRSPACE)
		{
			ULARGE_INTEGER fileSize;
			fileSize.LowPart = findData.nFileSizeLow;
			fileSize.HighPart = findData.nFileSizeHigh;
			std::wstring sizeStr = FormatNumber(fileSize.QuadPart);
			AddData(_FilesListBox, count, SIZE_COLUMN_INDEX, &sizeStr[0]);

			// format date/time
			FILETIME localFileTime;
			FileTimeToLocalFileTime(&findData.ftLastWriteTime, &localFileTime);
			SYSTEMTIME systemTime;
			FileTimeToSystemTime(&localFileTime, &systemTime);
			wchar_t dateTimeStr[64];
			swprintf_s(dateTimeStr, L"%04d-%02d-%02d %02d:%02d:%02d",
				systemTime.wYear,
				systemTime.wMonth,
				systemTime.wDay,
				systemTime.wHour,
				systemTime.wMinute,
				systemTime.wSecond);
			AddData(_FilesListBox, count, DATE_COLUMN_INDEX, dateTimeStr);
		}
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

	MessageBox(_T("File copied."), _T("Info"), MB_OK | MB_ICONINFORMATION);
}

void CCopyWSRDlg::OnBnClickedButtonDelete()
{
	// get the selected item
	int index = _FilesListBox.GetNextItem(-1, LVNI_SELECTED);
	if (index == -1)
		return;

	CString str = _FilesListBox.GetItemText(index, 0);
	CString fullPath = _Directory + _T("\\") + str;

	CWaitCursor wait;

	CommandWriter::writeDeleteFile(fullPath);

	RunService();
	UpdateFileListBox();

	MessageBox(_T("File deleted."), _T("Info"), MB_OK | MB_ICONINFORMATION);
}

void CCopyWSRDlg::OnBnClickedButtonRename()
{
	// get the selected item
	int index = _FilesListBox.GetNextItem(-1, LVNI_SELECTED);
	if (index == -1)
		return;

	CString oldFileName = _FilesListBox.GetItemText(index, 0);
	CString oldFullPath = _Directory + _T("\\") + oldFileName;

	CNewFileNameDlg dlg(this, oldFileName);
	if (dlg.DoModal() != IDOK)
		return;

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

void CCopyWSRDlg::OnBnClickedButtonBackup()
{
	int index = _FilesListBox.GetNextItem(-1, LVNI_SELECTED);
	if (index == -1)
		return;

	CString str = _FilesListBox.GetItemText(index, 0);
	CString fullSourcePath = _Directory + _T("\\") + str;

	CString fullDestPath = fullSourcePath + BACKUP_FILE_EXTENSION;

	CWaitCursor wait;

	CommandWriter::writeCopyFile(fullSourcePath, fullDestPath);

	RunService();
	UpdateFileListBox();

	MessageBox(_T("Backup completed."), _T("Info"), MB_OK | MB_ICONINFORMATION);
}

void CCopyWSRDlg::OnBnClickedButtonRestore()
{
	int index = _FilesListBox.GetNextItem(-1, LVNI_SELECTED);
	if (index == -1)
		return;

	CString str = _FilesListBox.GetItemText(index, 0);
	CString fullSourcePath = _Directory + _T("\\") + str;

	// remove .wsrbackup extension
	int extPos = fullSourcePath.ReverseFind('.');
	if (extPos == -1)
		return;

	CString fullDestPath = fullSourcePath.Left(extPos);

	CWaitCursor wait;

	CommandWriter::writeCopyFile(fullSourcePath, fullDestPath);

	RunService();
	UpdateFileListBox();

	MessageBox(_T("Restore completed."), _T("Info"), MB_OK | MB_ICONINFORMATION);
}

void CCopyWSRDlg::OnLvnItemchangedListFiles(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if (pNMLV->iItem == -1)
		return;

	CString str = _FilesListBox.GetItemText(pNMLV->iItem, 0);

	bool result = IsBackupFile(str);
	_RestoreBtn.EnableWindow(result);
}

BOOL CCopyWSRDlg::IsBackupFile(CString str)
{
	// if file ends with .wsrbackup

	int length = CString(BACKUP_FILE_EXTENSION).GetLength();
	int pos = str.Right(length).CompareNoCase(BACKUP_FILE_EXTENSION);

	return pos == 0;
}

void CCopyWSRDlg::OnBnClickedButtonBrowse()
{
	// dirctory browse dialog defaulting to c:\windows\system32
}

BOOL CCopyWSRDlg::DestroyWindow()
{
	CSettings settings;
	settings.SetPath(_Directory);
	settings.Save();

	return CDialogEx::DestroyWindow();
}
void CCopyWSRDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	// ensure the dialog does not get smaller than the minimum size
	if (pRect->right - pRect->left < _UIMinSize.cx)
		pRect->right = pRect->left + _UIMinSize.cx;

	if (pRect->bottom - pRect->top < _UIMinSize.cy)
		pRect->bottom = pRect->top + _UIMinSize.cy;

	CDialogEx::OnSizing(fwSide, pRect);
}