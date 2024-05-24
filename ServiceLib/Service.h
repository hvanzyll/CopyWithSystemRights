#pragma once

#include "ServiceWorker.h"

class CService
{
public:
	CService();
	DWORD Start(IServiceWorker* pServiceWorker);

private:
	IServiceWorker* m_pServiceWorker;

	SERVICE_STATUS        m_ServiceStatus;
	SERVICE_STATUS_HANDLE m_StatusHandle;
	HANDLE                m_ServiceStopEvent;

	static VOID WINAPI ServiceCtrlHandlerStub(DWORD CtrlCode);
	static DWORD WINAPI ServiceWorkerThreadStub(LPVOID lpParam);
	static VOID WINAPI ServiceMainStub(DWORD argc, LPTSTR* argv);

	bool ServiceMain(DWORD argc, LPTSTR* argv);
	void ServiceCtrlHandler(DWORD CtrlCode);
	DWORD ServiceWorkerThread(LPVOID lpParam);
};
