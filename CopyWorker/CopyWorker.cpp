#include "pch.h"
#include "CopyWorker.h"
#include <fstream>
#include <cstring>
#include <ShlObj_core.h>
#include <vector>

bool CopyWorker::RunWorker(DWORD& timeOutIntervalms)
{
	TCHAR data[4096];
	ZeroMemory(data, 4096 * sizeof(TCHAR));
	if (!readFile(data, 4096))
	{
		DWORD error = GetLastError();
		writeResponse(error);
		return false;
	}
	TCHAR logData[4096];
	ZeroMemory(logData, 4096 * sizeof(TCHAR));
	_tcscpy_s(logData, 4096, data);

	std::vector<TCHAR*> commands;
	// loop through the data split by new line and add to vector
	TCHAR* context = nullptr;
	TCHAR* command = wcstok_s(data, L"\n", &context);
	while (command != nullptr)
	{
		commands.push_back(command);
		command = wcstok_s(nullptr, L"\n", &context);
	}

	// check if command is copy
	if (_wcsicmp(commands[0], L"copy") == 0 && commands.size() == 3)
	{
		TCHAR* sourcePath = commands[1];
		TCHAR* destFolder = commands[2];
		// copy the file
		CopyFile(sourcePath, destFolder, FALSE);

		// get last error
		DWORD error = GetLastError();
		writeResponse(error);
	}
	// check if command is delete
	else if (_wcsicmp(commands[0], L"delete") == 0 && commands.size() == 2)
	{
		TCHAR* fullPath = commands[1];
		// delete the file
		DeleteFile(fullPath);

		// get last error
		DWORD error = GetLastError();
		writeResponse(error);
	}
	else if (_wcsicmp(commands[0], L"rename") == 0 && commands.size() == 3)
	{
		TCHAR* oldFullPath = commands[1];
		TCHAR* newFullPath = commands[2];
		// rename the file
		MoveFile(oldFullPath, newFullPath);

		// get last error
		DWORD error = GetLastError();
		writeResponse(error);
	}
	else
	{
		// append data to "Invalid command" then write to response file
		TCHAR out[4096];
		ZeroMemory(out, 4096 * sizeof(TCHAR));

		_tcscat_s(out, 4096, L"-1 Invalid command\n");
		_tcscat_s(out, 4096, logData);
		writeResponse(out);
	}

	return false;
}

void CopyWorker::writeResponse(DWORD error)
{
	const int MAX_ERROR = 4096;
	// convert error to string
	LPWSTR errorText = nullptr;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&errorText, 0, NULL);

	// prepend the error code to the error message
	TCHAR errorTextWithCode[MAX_ERROR];
	ZeroMemory(errorTextWithCode, MAX_ERROR * sizeof(TCHAR));
	_itow_s(error, errorTextWithCode, MAX_ERROR, 10);
	_tcscat_s(errorTextWithCode, MAX_ERROR, L": ");
	_tcscat_s(errorTextWithCode, MAX_ERROR, errorText);
	// free the error text
	LocalFree(errorText);

	// write the error to the response file
	writeResponse(errorTextWithCode);
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

void CopyWorker::writeResponse(const TCHAR* Data)
{
	TCHAR path[MAX_PATH];
	getResponseFilePath(path, MAX_PATH);

	// open the file
	std::wofstream file;
	file.open(path, std::ios::out);
	if (file.is_open())
	{
		// write the data
		file.write(Data, wcslen(Data));

		// close the file
		file.close();
	}
	else
	{
		DWORD error = GetLastError();

		const int MAX_ERROR = 4096;
		// convert error to string
		LPWSTR errorText = nullptr;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&errorText, 0, NULL);

		LocalFree(errorText);
	}
}
void CopyWorker::getSourceFilePath(TCHAR* path, int size)
{
	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path);

	// append the file name to the current directory
	_tcscat_s(path, size, L"\\hvanzyll\\CopyWSR\\CopyWSRCommand.txt");
}

void CopyWorker::getResponseFilePath(TCHAR* path, int size)
{
	SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path);

	// append the file name to the current directory
	_tcscat_s(path, size, L"\\hvanzyll\\CopyWSR\\CopyWSRResponse.txt");
}