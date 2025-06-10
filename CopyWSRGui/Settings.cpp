#include "pch.h"
#include "Settings.h"

CSettings::CSettings()
{
}
CSettings::~CSettings()
{
}

using namespace std;

//
// Returns the file portion from a path
//
CString GetFileOnly(LPCTSTR Path)
{
	// Strip off the path and return just the filename part
	CString temp = (LPCTSTR)Path; // Force CString to make a copy
	::PathStripPath(temp.GetBuffer(0));
	temp.ReleaseBuffer();
	return temp;
}

//
// Returns the folder portion from a path
//
CString GetFolderOnly(LPCTSTR Path)
{
	// Strip off the file name so we can direct the file scanning dialog to	go
	// back to the same directory as before.
	CString temp = (LPCTSTR)Path; // Force CString to make a copy
	::PathRemoveFileSpec(temp.GetBuffer(0));
	temp.ReleaseBuffer();
	return temp;
}

//
// Adds a folder path and file together to make a file path
//
CString AddPathAndFile(LPCTSTR Folder, LPCTSTR File)
{
	CString cs = Folder;
	::PathAddBackslash(cs.GetBuffer(_MAX_PATH));
	::PathAppend(cs.GetBuffer(_MAX_PATH), File);
	cs.ReleaseBuffer();
	return cs;
}

//
// Adds a .ext to the end of a file path
//
CString AddExt(LPCTSTR Path, LPCTSTR Ext)
{
	CString cs = Path;
	::PathAddExtension(cs.GetBuffer(_MAX_PATH), Ext);
	cs.ReleaseBuffer();
	return cs;
}

//
// Returns true if file exists or false if file does not exist
//
BOOL FileExists(LPCTSTR Path)
{
	return (::PathFileExists(Path));
}

//
// Returns just the .ext part of the file path
//
CString GetFileExt(LPCTSTR Path)
{
	CString cs;
	cs = ::PathFindExtension(Path);
	return cs;
}

CString GetFileName(LPCTSTR Path)
{
	CString csFile = GetFileOnly(Path);
	if (!GetFileExt(csFile).IsEmpty()) { // Is there an extension
		::PathRemoveExtension(csFile.GetBuffer(_MAX_PATH));
		csFile.ReleaseBuffer();
	}
	return csFile;
}

void CreateFolder(CString& cs)
{
	::CreateDirectory(cs.GetBuffer(_MAX_PATH), NULL);
	cs.ReleaseBuffer();
}

bool CSettings::Load()
{
	CString path;
	if (!GetPath(path, L"settings.txt"))
		return false;

	if (!FileExists(path))
		return false;

	// read file
	CFile theFile(path, CFile::modeRead);

	// sanity check
	if (theFile.GetLength() > static_cast<unsigned long long>(8 * 1024))
		return false;

	CString settingsTxt;
	int fileLength = (int)theFile.GetLength();
	theFile.Read(settingsTxt.GetBuffer(fileLength), fileLength);
	settingsTxt.ReleaseBuffer(fileLength);

	StringToSettings(settingsTxt);

	return true;
}

bool CSettings::Save()
{
	CString path;
	if (!GetPath(path, L"settings.txt"))
		return false;

	CString folder = GetFolderOnly(path);
	CreateFolder(folder);

	CString settingsTxt = SettingsToString();

	TRY
	{
		// write file
		CFile theFile(path, CFile::modeReadWrite | CFile::modeCreate);
		theFile.Write((LPCTSTR)settingsTxt, settingsTxt.GetLength() * sizeof(TCHAR));

		WCHAR endfile = '\0';
		theFile.Write(&endfile, sizeof(WCHAR));
	}
		CATCH(CException, e)
	{
		return false;
	}
	END_CATCH

		return true;
}

bool CSettings::GetPath(CString& path, CString filename)
{
	TCHAR szPath[MAX_PATH];
	// Get path for each computer, non-user specific and non-roaming data.
	if (FAILED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
		return false;

	TCHAR buf[MAX_PATH];
	GetModuleFileName(nullptr, buf, MAX_PATH);
	CString temp = GetFileName(buf);

	CString folder = szPath;
	path = AddPathAndFile(folder, L"hvanzyll");
	path = AddPathAndFile(path, temp);
	path = AddPathAndFile(path, filename);

	return true;
}

CString CSettings::SettingsToString()
{
	CString out;

	out += L"LastPath";
	out += L":";
	out += m_Path;
	out += L"\n";

	return out;
}

void CSettings::StringToSettings(CString settingsTxt)
{
	int pos = settingsTxt.Find(L"\n");
	if (pos <= 0)
		return;

	CString line = settingsTxt.Left(pos);
	CString trimmedSettingsTxt = settingsTxt.Mid(pos + 1, settingsTxt.GetLength());

	pos = line.Find(L":");
	if (pos <= 0)
		return;

	CString settingEntry = line.Mid(0, pos);
	CString settingValue = line.Mid(pos + 1);

	if (settingEntry == L"LastPath")
	{
		m_Path = settingValue;
	}
}