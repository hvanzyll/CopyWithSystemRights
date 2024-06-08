#pragma once

#include "../ServiceLib/ServiceWorker.h"

class CopyWorker : public IServiceWorker
{
public:
	// Inherited via IServiceWorker
	bool RunWorker(DWORD& timeOutIntervalms) override;

protected:
	bool readFile(TCHAR* Data, int size);
	void writeResponse(DWORD error);
	void writeResponse(const TCHAR* Data);
	void getSourceFilePath(TCHAR* path, int size);
	void getResponseFilePath(TCHAR* path, int size);
};
