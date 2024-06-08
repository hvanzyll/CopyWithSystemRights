#pragma once

class CommandWriter
{
public:
	static void writeCopyFile(const TCHAR* sourcePath, const TCHAR* destFolder);
	static void writeDeleteFile(const TCHAR* fullPath);
	static void writeRenameFile(const TCHAR* oldFullPath, const TCHAR* newFullPath);

	static bool readResposeFile(TCHAR* data, int size);

	static void getServiceFilePath(TCHAR* path, int size);
	static void deleteCommandFile();
	static void deleteCommandDir();
	static void deleteResponseFile();

protected:
	static void getCommandFilePath(TCHAR* path, int size);
	static void getResponseFilePath(TCHAR* path, int size);
	static void getCreateAppDataPath(TCHAR* path, int size);
};
