// CopyWSR.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "CopyWSR.h"

#include "../ServiceLib/Service.h"
#include "../ServiceLib/ServiceControl.h"
#pragma comment(lib, "SimpleServiceLib.lib")

#include "../CopyWorker/CopyWorker.h"
#pragma comment(lib, "CopyWorker.lib")

// Forward declarations of functions included in this code module:

int _tmain(int argc, TCHAR* argv[])
{
	int ret = 0;
	if (argc == 1)
	{
		CopyWorker worker;
		CService svc;
		ret = svc.Start(&worker);
	}
	if (argc == 2)
	{
		if (_tcsicmp(argv[1], _T("/register")) == 0)
		{
			ret = ServiceControl::registerService(argv[0], _T("CopyWSRSvc"), _T("Copy With System Rights Service"));
		}
		else if (_tcsicmp(argv[1], _T("/unregister")) == 0)
		{
			ret = ServiceControl::unregisterService(_T("CopyWSRSvc"));
		}
		else
		{
			ret = -1;
		}
	}
	return ret;
}