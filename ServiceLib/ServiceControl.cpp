#include "pch.h"
#include "ServiceControl.h"

BOOL ServiceControl::isServiceRegistered(TCHAR* szServiceName)
{
	SC_HANDLE managerHnd = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CREATE_SERVICE | SERVICE_START);
	if (managerHnd == NULL)
		return FALSE;

	SC_HANDLE serviceHnd = OpenService(managerHnd, szServiceName, SERVICE_QUERY_CONFIG);
	if (serviceHnd == NULL)
	{
		CloseServiceHandle(managerHnd);
		return FALSE;
	}

	CloseServiceHandle(serviceHnd);
	CloseServiceHandle(managerHnd);

	return TRUE;
}

UINT ServiceControl::registerService(const TCHAR* szServicePath, const  TCHAR* szServiceName, const TCHAR* szDisplayName)
{
	SC_HANDLE managerHnd = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CREATE_SERVICE | SERVICE_START);
	if (managerHnd == NULL)
		return GetLastError();

	SC_HANDLE serviceHnd = CreateService(managerHnd, szServiceName, szDisplayName, SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, szServicePath,
		NULL, NULL, NULL, NULL, NULL);
	if (serviceHnd == NULL)
	{
		CloseServiceHandle(managerHnd);
		return GetLastError();
	}

	CloseServiceHandle(serviceHnd);
	CloseServiceHandle(managerHnd);

	return ERROR_SUCCESS;
}

UINT ServiceControl::unregisterService(const TCHAR* szServiceName)
{
	SC_HANDLE managerHnd = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CREATE_SERVICE | SERVICE_START);
	if (managerHnd == NULL)
		return GetLastError();

	SC_HANDLE serviceHnd = OpenService(managerHnd, szServiceName, DELETE);
	if (serviceHnd == NULL)
	{
		CloseServiceHandle(managerHnd);
		return GetLastError();
	}

	BOOL ret = DeleteService(serviceHnd);
	if (!ret)
	{
		CloseServiceHandle(serviceHnd);
		CloseServiceHandle(managerHnd);
		return GetLastError();
	}

	CloseServiceHandle(managerHnd);

	return ERROR_SUCCESS;
}

UINT ServiceControl::startService(wchar_t* szServiceName)
{
	SERVICE_STATUS_PROCESS ssStatus;
	DWORD dwOldCheckPoint;
	ULONGLONG dwStartTickCount;
	DWORD dwWaitTime;
	DWORD dwBytesNeeded;

	// Get a handle to the SCM database.

	SC_HANDLE schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // servicesActive database
		SC_MANAGER_ALL_ACCESS);  // full access rights

	if (NULL == schSCManager)
	{
		//printf("OpenSCManager failed (%d)\n", GetLastError());
		return GetLastError();
	}

	// Get a handle to the service.

	SC_HANDLE schService = OpenService(
		schSCManager,         // SCM database
		szServiceName,            // name of service
		SERVICE_ALL_ACCESS);  // full access

	if (schService == NULL)
	{
		//printf("OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return  GetLastError();
	}

	// Check the status in case the service is not stopped.

	if (!QueryServiceStatusEx(
		schService,                     // handle to service
		SC_STATUS_PROCESS_INFO,         // information level
		(LPBYTE)&ssStatus,             // address of structure
		sizeof(SERVICE_STATUS_PROCESS), // size of structure
		&dwBytesNeeded))              // size needed if buffer is too small
	{
		//printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return GetLastError();
	}

	// Check if the service is already running. It would be possible
	// to stop the service here, but for simplicity this example just returns.

	if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
	{
		//printf("Cannot start the service because it is already running\n");
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return GetLastError();
	}

	// Save the tick count and initial checkpoint.

	dwStartTickCount = GetTickCount64();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	// Wait for the service to stop before attempting to start it.

	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
	{
		// Do not wait longer than the wait hint. A good interval is
		// one-tenth of the wait hint but not less than 1 second
		// and not more than 10 seconds.

		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		// Check the status until the service is no longer stop pending.

		if (!QueryServiceStatusEx(
			schService,                     // handle to service
			SC_STATUS_PROCESS_INFO,         // information level
			(LPBYTE)&ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded))              // size needed if buffer is too small
		{
			//printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			CloseServiceHandle(schService);
			CloseServiceHandle(schSCManager);
			return GetLastError();
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			// Continue to wait and check.

			dwStartTickCount = GetTickCount64();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if (GetTickCount64() - dwStartTickCount > ssStatus.dwWaitHint)
			{
				//printf("Timeout waiting for service to stop\n");
				CloseServiceHandle(schService);
				CloseServiceHandle(schSCManager);
				return GetLastError();
			}
		}
	}

	// Attempt to start the service.

	if (!::StartService(
		schService,  // handle to service
		0,           // number of arguments
		NULL))      // no arguments
	{
		//printf("StartService failed (%d)\n", GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return GetLastError();
	}
	
	//printf("Service start pending...\n");

	//Check the status until the service is no longer start pending.
	if (!QueryServiceStatusEx(
		schService,                     // handle to service
		SC_STATUS_PROCESS_INFO,         // info level
		(LPBYTE)&ssStatus,             // address of structure
		sizeof(SERVICE_STATUS_PROCESS), // size of structure
		&dwBytesNeeded))              // if buffer too small
	{
		//printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return GetLastError();
	}

	// Save the tick count and initial checkpoint.

	dwStartTickCount = GetTickCount64();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
	{
		// Do not wait longer than the wait hint. A good interval is
		// one-tenth the wait hint, but no less than 1 second and no
		// more than 10 seconds.

		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		// Check the status again.

		if (!QueryServiceStatusEx(
			schService,             // handle to service
			SC_STATUS_PROCESS_INFO, // info level
			(LPBYTE)&ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded))              // if buffer too small
		{
			//printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			break;
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			// Continue to wait and check.

			dwStartTickCount = GetTickCount64();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if (GetTickCount64() - dwStartTickCount > ssStatus.dwWaitHint)
			{
				// No progress made within the wait hint.
				break;
			}
		}
	}

	// Determine whether the service is running.

	/*if (ssStatus.dwCurrentState == SERVICE_RUNNING)
	{
		printf("Service started successfully.\n");
	}
	else
	{
		printf("Service not started. \n");
		printf("  Current State: %d\n", ssStatus.dwCurrentState);
		printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
		printf("  Check Point: %d\n", ssStatus.dwCheckPoint);
		printf("  Wait Hint: %d\n", ssStatus.dwWaitHint);
	}*/

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

	// service could be finished and no longer running
	return ERROR_SUCCESS;
}

UINT ServiceControl::stopService(TCHAR* szServiceName)
{
	SC_HANDLE managerHnd = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SERVICE_START);
	if (managerHnd == NULL)
		return GetLastError();

	SC_HANDLE serviceHnd = OpenService(managerHnd, szServiceName, SERVICE_STOP);
	if (managerHnd == NULL)
	{
		CloseServiceHandle(managerHnd);
		return GetLastError();
	}
	SERVICE_STATUS_PROCESS ssp = { 0 };
	BOOL ret = ControlService(
		serviceHnd,
		SERVICE_CONTROL_STOP,
		(LPSERVICE_STATUS)&ssp);
	{
		//printf("ControlService failed (%d)\n", GetLastError());
		//goto stop_cleanup;

		return GetLastError();
	}
	if (!ret)
	{
		CloseServiceHandle(serviceHnd);
		CloseServiceHandle(managerHnd);
		return GetLastError();
	}

	CloseServiceHandle(serviceHnd);
	CloseServiceHandle(managerHnd);

	return ERROR_SUCCESS;
}