#include "pch.h"
#include "CommandWriter.h"
#include <fstream>

void CommandWriter::writeCopyFile(const TCHAR* sourcePath, const TCHAR* destFolder)
{
	TCHAR filePath[MAX_PATH];
	getCommandFilePath(filePath, MAX_PATH);

	std::wofstream file;
	file.open(filePath, std::ios::out | std::ios::trunc);
	if (file.is_open())
	{
		file << "copy" << std::endl;
		file << std::wstring(sourcePath).c_str() << std::endl;
		file << std::wstring(destFolder).c_str() << std::endl;
		file.close();
	}
}

void CommandWriter::writeDeleteFile(const TCHAR* fullPath)
{
	TCHAR filePath[MAX_PATH];
	getCommandFilePath(filePath, MAX_PATH);

	std::wofstream file;
	file.open(filePath, std::ios::out | std::ios::trunc);
	if (file.is_open())
	{
		file << "delete" << std::endl;
		file << std::wstring(fullPath).c_str() << std::endl; // Convert TCHAR* to std::wstring
		file.close();
	}
}

void CommandWriter::writeRenameFile(const TCHAR* oldFullPath, const TCHAR* newFullPath)
{
	TCHAR filePath[MAX_PATH];
	getCommandFilePath(filePath, MAX_PATH);

	std::wofstream file;
	file.open(filePath, std::ios::out | std::ios::trunc);
	if (file.is_open())
	{
		file << "rename" << std::endl;
		file << std::wstring(oldFullPath).c_str() << std::endl;
		file << std::wstring(newFullPath).c_str() << std::endl;
		file.close();
	}
}

void CommandWriter::getCommandFilePath(TCHAR* path, int size)
{
	getCreateAppDataPath(path, size);

	_tcscat_s(path, size, L"\\CopyWSRCommand.txt");
}

void CommandWriter::getServiceFilePath(TCHAR* path, int size)
{
	getCreateAppDataPath(path, size);

	_tcscat_s(path, size, L"\\CopyWSRSvc.exe");
}

void CommandWriter::deleteCommandFile()
{
	TCHAR filePath[MAX_PATH];
	getCommandFilePath(filePath, MAX_PATH);

	DeleteFile(filePath);
}

void CommandWriter::deleteCommandDir()
{
	TCHAR path[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path);

	// append the file name to the current directory
	_tcscat_s(path, MAX_PATH, L"\\hvanzyll\\CopyWSR");

	RemoveDirectory(path);
}

void CommandWriter::getCreateAppDataPath(TCHAR* path, int size)
{
	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path);

	// append the file name to the current directory
	_tcscat_s(path, size, L"\\hvanzyll\\CopyWSR");

	SHCreateDirectoryEx(NULL, path, NULL);
}