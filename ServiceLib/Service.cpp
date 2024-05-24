#include "pch.h"
#include "Service.h"
#include <stdio.h>

#define SERVICE_NAME  _T("Copy With System Rights Service")

CService* g_pService = NULL;

CService::CService()
	: m_ServiceStopEvent(INVALID_HANDLE_VALUE),
	m_StatusHandle(NULL),
	m_pServiceWorker(NULL)
{
	ZeroMemory(&m_ServiceStatus, sizeof(m_ServiceStatus));
	g_pService = this;
}

DWORD CService::Start(IServiceWorker* pServiceWorker)
{
	if (pServiceWorker == NULL)
		return ERROR_INVALID_HANDLE;

	m_pServiceWorker = pServiceWorker;

	SERVICE_TABLE_ENTRY ServiceTable[] =
	{
		{(LPWSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMainStub},
		{NULL, NULL}
	};

	if (StartServiceCtrlDispatcher(ServiceTable) == FALSE)
	{
		return GetLastError();
	}

	return 0;
}

VOID WINAPI CService::ServiceMainStub(DWORD argc, LPTSTR* argv)
{
	g_pService->ServiceMain(argc, argv);
}

VOID WINAPI CService::ServiceCtrlHandlerStub(DWORD CtrlCode)
{
	g_pService->ServiceCtrlHandler(CtrlCode);
}

DWORD WINAPI CService::ServiceWorkerThreadStub(LPVOID lpParam)
{
	return g_pService->ServiceWorkerThread(lpParam);
}

bool CService::ServiceMain(DWORD argc, LPTSTR* argv)
{
	DWORD Status = E_FAIL;

	OutputDebugString(_T("ServiceMain: Entry"));

	m_StatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandlerStub);

	if (m_StatusHandle == NULL)
	{
		OutputDebugString(_T("ServiceMain: RegisterServiceCtrlHandler returned error"));
		return false;
	}

	// Tell the service controller we are starting
	ZeroMemory(&m_ServiceStatus, sizeof(m_ServiceStatus));
	m_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	m_ServiceStatus.dwControlsAccepted = 0;
	m_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	m_ServiceStatus.dwWin32ExitCode = 0;
	m_ServiceStatus.dwServiceSpecificExitCode = 0;
	m_ServiceStatus.dwCheckPoint = 0;

	if (SetServiceStatus(m_StatusHandle, &m_ServiceStatus) == FALSE)
	{
		OutputDebugString(_T("ServiceMain: SetServiceStatus returned error"));
	}

	/*
	 * Perform tasks necessary to start the service here
	 */
	OutputDebugString(_T("ServiceMain: Performing Service Start Operations"));

	// Create stop event to wait on later.
	m_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_ServiceStopEvent == NULL)
	{
		OutputDebugString(_T("ServiceMain: CreateEvent(g_ServiceStopEvent) returned error"));

		m_ServiceStatus.dwControlsAccepted = 0;
		m_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		m_ServiceStatus.dwWin32ExitCode = GetLastError();
		m_ServiceStatus.dwCheckPoint = 1;

		if (SetServiceStatus(m_StatusHandle, &m_ServiceStatus) == FALSE)
		{
			OutputDebugString(_T("ServiceMain: SetServiceStatus returned error"));
		}
		return false;
	}

	// Tell the service controller we are started
	m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	m_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	m_ServiceStatus.dwWin32ExitCode = 0;
	m_ServiceStatus.dwCheckPoint = 0;

	if (SetServiceStatus(m_StatusHandle, &m_ServiceStatus) == FALSE)
	{
		OutputDebugString(_T("ServiceMain: SetServiceStatus returned error"));
	}

	// Start the thread that will perform the main task of the service
	HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThreadStub, NULL, 0, NULL);
	if (hThread != NULL)
	{
		OutputDebugString(_T("ServiceMain: Waiting for Worker Thread to complete"));

		// Wait until our worker thread exits effectively signaling that the service needs to stop
		WaitForSingleObject(hThread, INFINITE);

		OutputDebugString(_T("ServiceMain: Worker Thread Stop Event signaled"));
	}
	/*
	 * Perform any cleanup tasks
	 */
	OutputDebugString(_T("ServiceMain: Performing Cleanup Operations"));

	CloseHandle(m_ServiceStopEvent);

	m_ServiceStatus.dwControlsAccepted = 0;
	m_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	m_ServiceStatus.dwWin32ExitCode = 0;
	m_ServiceStatus.dwCheckPoint = 3;

	if (SetServiceStatus(m_StatusHandle, &m_ServiceStatus) == FALSE)
	{
		OutputDebugString(_T("ServiceMain: SetServiceStatus returned error"));
	}

	return true;
}

void CService::ServiceCtrlHandler(DWORD CtrlCode)
{
	OutputDebugString(_T("ServiceCtrlHandler: Entry"));

	switch (CtrlCode)
	{
	case SERVICE_CONTROL_STOP:

		OutputDebugString(_T("ServiceCtrlHandler: SERVICE_CONTROL_STOP Request"));

		if (m_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
			break;

		/*
		 * Perform tasks necessary to stop the service here
		 */

		m_ServiceStatus.dwControlsAccepted = 0;
		m_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		m_ServiceStatus.dwWin32ExitCode = 0;
		m_ServiceStatus.dwCheckPoint = 4;

		if (SetServiceStatus(m_StatusHandle, &m_ServiceStatus) == FALSE)
		{
			OutputDebugString(_T("ServiceCtrlHandler: SetServiceStatus returned error"));
		}

		// This will signal the worker thread to start shutting down
		SetEvent(m_ServiceStopEvent);

		break;

	default:
		break;
	}

	OutputDebugString(_T("ServiceCtrlHandler: Exit"));
}

DWORD CService::ServiceWorkerThread(LPVOID lpParam)
{
	DWORD timeOutIntervalms = 0;
	bool moreWork = true;
	//  Periodically check if the service has been requested to stop
	while (moreWork && WaitForSingleObject(m_ServiceStopEvent, timeOutIntervalms) != WAIT_OBJECT_0)
	{
		moreWork = m_pServiceWorker->RunWorker(timeOutIntervalms);
	}

	return ERROR_SUCCESS;
}