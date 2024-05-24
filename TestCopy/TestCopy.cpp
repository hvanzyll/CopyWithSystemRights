// TestCopy.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>

#include <iostream>
#include "..\CopyWorker\CopyWorker.h"
#pragma comment(lib, "CopyWorker.lib")

int main()
{
	DWORD timeOutIntervalms = 0;
	CopyWorker worker;
	worker.RunWorker(timeOutIntervalms);
	std::cout << "Hello World!\n";
}