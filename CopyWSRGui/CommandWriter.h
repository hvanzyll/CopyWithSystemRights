#pragma once

class CommandWriter
{
public:
	static void writeCopyFile(const TCHAR* sourcePath, const TCHAR* destFolder);
	static void writeDeleteFile(const TCHAR* fullPath);
	static void writeRenameFile(const TCHAR* oldFullPath, const TCHAR* newFullPath);

	static void getServiceFilePath(TCHAR* path, int size);
	static void deleteCommandFile();
	static void deleteCommandDir();
	static void getCommandFilePath(TCHAR* path, int size);

protected:
	static void getCreateAppDataPath(TCHAR* path, int size);
};
