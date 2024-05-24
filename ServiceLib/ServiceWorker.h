#pragma once

__interface IServiceWorker
{
	virtual bool RunWorker(DWORD& timeOutIntervalms) = 0;
};
