#pragma once
class ServiceControl
{
public:
	static BOOL isServiceRegistered(TCHAR* szServiceName);
	static UINT registerService(const TCHAR* szServicePath, const TCHAR* szServiceName, const  TCHAR* szDisplayName);
	static UINT unregisterService(const TCHAR* szServiceName);
	static UINT startService(wchar_t* szServiceName);
	static UINT stopService(TCHAR* szServiceName);
};
