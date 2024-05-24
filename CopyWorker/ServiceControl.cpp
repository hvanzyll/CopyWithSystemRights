#include "pch.h"
#include "ServiceControl.h"

//UINT ServiceControl::RegisterService(TCHAR* szServicePath, TCHAR* szServiceName, TCHAR* szDisplayName)
//{
//	SC_HANDLE managerHnd = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CREATE_SERVICE | SERVICE_START);
//	if (managerHnd == NULL)
//		return GetLastError();
//
//	SC_HANDLE serviceHnd = CreateService(managerHnd, szServiceName, szDisplayName, SERVICE_ALL_ACCESS,
//		SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, szServicePath,
//		NULL, NULL, NULL, NULL, NULL);
//	if (serviceHnd == NULL)
//	{
//		CloseServiceHandle(managerHnd);
//		return GetLastError();
//	}
//
//	BOOL ret = ::StartService(serviceHnd, 0, NULL);
//	if (!ret)
//	{
//		CloseServiceHandle(serviceHnd);
//		CloseServiceHandle(managerHnd);
//		return GetLastError();
//	}
//
//	CloseServiceHandle(managerHnd);
//
//	return ERROR_SUCCESS;
//}