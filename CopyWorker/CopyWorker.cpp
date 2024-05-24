#include "pch.h"
#include "CopyWorker.h"
#include <fstream>
#include <cstring>
#include <ShlObj_core.h>

bool CopyWorker::RunWorker(DWORD& timeOutIntervalms)
{
	TCHAR data[4096];
	ZeroMemory(data, 4096 * sizeof(TCHAR));
	readFile(data, 4096);

	// split data by lines
	TCHAR* context = nullptr;
	TCHAR* command = wcstok_s(data, L"\n", &context);
	if (command == nullptr) {
		return false;
	}

	// check if command is copy
	if (_wcsicmp(command, L"copy") == 0)
	{
		TCHAR* sourcePath = wcstok_s(nullptr, L"\n", &context);
		TCHAR* destFolder = wcstok_s(nullptr, L"\n", &context);
		// copy the file
		bool ret = CopyFile(sourcePath, destFolder, FALSE);
	}
	// check if command is delete
	else if (_wcsicmp(command, L"delete") == 0)
	{
		TCHAR* fullPath = wcstok_s(nullptr, L"\n", &context);
		// delete the file
		DeleteFile(fullPath);
	}
	else if (_wcsicmp(command, L"rename") == 0)
	{
		TCHAR* oldFullPath = wcstok_s(nullptr, L"\n", &context);
		TCHAR* newFullPath = wcstok_s(nullptr, L"\n", &context);
		// rename the file
		MoveFile(oldFullPath, newFullPath);
	}

	return false;
}

bool CopyWorker::readFile(TCHAR* Data, int size)
{
	TCHAR path[MAX_PATH];
	getSourceFilePath(path, MAX_PATH);

	// open the file
	std::wifstream file;
	file.open(path, std::ios::in);
	if (!file.is_open()) {
		// if the file does not exist, return an empty string
		Data[0] = L'\0';
		return false;
	}
	// read the file
	file.read(Data, size);
	// close the file
	file.close();

	return true;
}

void CopyWorker::getSourceFilePath(TCHAR* path, int size)
{
	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path);

	// append the file name to the current directory
	_tcscat_s(path, size, L"\\hvanzyll\\CopyWSR\\CopyWSRCommand.txt");
}