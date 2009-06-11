//-----------------------------------------------------------------------------
// Copyright 2009 Vadim Macagon
// 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
// 
// http://www.apache.org/licenses/LICENSE-2.0 
// 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.
//-----------------------------------------------------------------------------
#include "precompiled.h"
#if DBG == 1
#include "trace.h"
#include "main.tmh"
#endif

HINSTANCE hDllInstance;
const wchar_t* moduleName = L"PrintJackUI";

//-----------------------------------------------------------------------------
/**
	@brief DLL entry point.
*/
BOOL
WINAPI 
DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpRes)
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			hDllInstance = hModule;
			DisableThreadLibraryCalls(hModule);
			InitCommonControls();
			INITCOMMONCONTROLSEX icc;
			icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
			icc.dwICC = ICC_STANDARD_CLASSES;
			InitCommonControlsEx(&icc);
			WPP_INIT_TRACING(moduleName);
			return TRUE;
		}

		case DLL_PROCESS_DETACH:
			WPP_CLEANUP();
			return TRUE;
	}

	UNREFERENCED_PARAMETER(lpRes);

	return TRUE;
}

//-----------------------------------------------------------------------------
/**
	@brief Construct a special printer name that can be used with OpenPrinter()
	       to establish a connection to a port monitor server dll.
	@param serverName Name of the server, can be NULL.
	@param objectType One of the following:
	                  - XcvMonitor
	                  - XcvPort
	@param monitorName Name of the monitor, can be NULL.
	@return A string of the form '\\ServerName\,ObjectType MonitorName'.
*/
std::wstring
ConstructXcvName(const wchar_t* serverName, const wchar_t* objectType, 
                 const wchar_t* monitorName)
{
	std::wstring xcvName;
	
	if (serverName)
	{
		xcvName = serverName;
		xcvName += L"\\,";
	}
	else
	{
		xcvName = L",";
	}

	xcvName += objectType;
	xcvName += L" ";
	
	if (monitorName)
		xcvName += monitorName;

	return xcvName;
}

//-----------------------------------------------------------------------------
/**
	@param appPathMaxSize Maximum number of WCHAR(s) that can be stored in 
	                      the appPath buffer.
*/
bool
GetConfigAppPath(wchar_t* appPath, int appPathMaxSize)
{
	HKEY configAppKey = NULL;
	DWORD retval = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE, L"SOFTWARE\\PrintJack",
		0, KEY_READ, &configAppKey
	);
	if ((retval != ERROR_SUCCESS) || !configAppKey)
		return FALSE;
		
	wchar_t pathValueName[] = L"Path";
	DWORD valueType = 0;
	DWORD outputSize = appPathMaxSize * sizeof(wchar_t);
	retval = RegQueryValueEx(
		configAppKey, pathValueName, 0,
		&valueType, (BYTE*)appPath, &outputSize
	);
	if (retval != ERROR_SUCCESS)
		return FALSE;
	if ((valueType != REG_SZ) && (valueType != REG_EXPAND_SZ))
		return FALSE;
	// ensure the string is null-terminated
	appPath[appPathMaxSize - 1] = 0;
	return TRUE;
}

//-----------------------------------------------------------------------------
/**
	@param portName Port name, can be NULL.
	@param cmdLineMaxSize Maximum number of WCHAR(s) that can be stored in the
	                      cmdLine buffer.
*/
bool
BuildCommandLine(const wchar_t* appPath, const wchar_t* action, 
                 const wchar_t* portName,
                 wchar_t* cmdLine, int cmdLineMaxSize)
{
	// extract the application name from the path
	const wchar_t* appName = appPath;
	for (size_t i = wcslen(appPath); i != 0; --i)
	{
		if (('\\' == appPath[i]) || ('/' == appPath[i]))
		{
			if (i < wcslen(appPath))
				appName = &appPath[i + 1];
			else
				return FALSE;
		}
	}
	if (wcslen(appName) == 0)
		return FALSE;
		
	if (portName)
		return swprintf_s(cmdLine, cmdLineMaxSize, L"%s -%s \"%s\"", appName, action, portName) > 0;
	else
		return swprintf_s(cmdLine, cmdLineMaxSize, L"%s -%s", appName, action) > 0;
}

//-----------------------------------------------------------------------------
/**
	
*/
BOOL
LaunchConfigApp(const wchar_t* action, const wchar_t* portName)
{
	const int appPathMaxSize = 1024;
	wchar_t appPath[appPathMaxSize];
	if (!GetConfigAppPath(appPath, appPathMaxSize))
		return FALSE;
		
	const int cmdLineMaxSize = 512;
	wchar_t cmdLine[cmdLineMaxSize];
	if (!BuildCommandLine(appPath, action, portName, cmdLine, cmdLineMaxSize))
		return FALSE;
		
	BOOL retval = FALSE;
	
	STARTUPINFO startUpInfo;
	ZeroMemory(&startUpInfo, sizeof(STARTUPINFO));
	startUpInfo.cb = sizeof(STARTUPINFO);
			
	PROCESS_INFORMATION processInfo;
	ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));
			
	retval = CreateProcess(
		appPath,
		cmdLine,
		NULL, // process security attributes 
		NULL, // primary thread security attributes 
		FALSE, // don't inherit handles
		CREATE_UNICODE_ENVIRONMENT | DETACHED_PROCESS, // creation flags
		NULL, // use parent environment
		NULL, // use parent current directory 
		&startUpInfo,
		&processInfo
	);
	
	if (retval)
	{
		WaitForInputIdle(processInfo.hProcess, 1000);
		CloseHandle(processInfo.hThread);
		CloseHandle(processInfo.hProcess);
	}
		
	return retval;
}

//-----------------------------------------------------------------------------
/**
	@brief Add a printer port, then obtain port configuration information from 
	       the user and send it to the port monitor server DLL.
	@param pszServer The name of the server, can be NULL.
	@param hWnd Parent window for dialog boxes, if NULL no dialog boxes should
	            be displayed.
	@param pszPortNameIn The name of the print monitor, can be NULL.
	@param ppszPortNameOut Pointer to a location where the port name string
	                       should be stored, can be NULL.
	@return TRUE if the port was successfully added, FALSE otherwise.
*/
BOOL
WINAPI
AddPortUI(PCWSTR pszServer, HWND hWnd, PCWSTR pszPortNameIn, PWSTR* ppszPortNameOut)
{
	// fixme: should probably do something with ppszPortNameOut
	/*
	if (ppszPortNameOut)
	{
		// need this ugliness because we have to pass the string back
		// to the caller who presumably will use GlobalFree to cleanup
		size_t portNameSize = (wcslen(portName) + 1) * sizeof(WCHAR);
		WCHAR* portNameOut = (WCHAR *)GlobalAlloc(GMEM_FIXED, portNameSize);
		wcscpy_s(portNameOut, portNameSize, portName);
		*ppszPortNameOut = portNameOut;
	}
	*/
	return LaunchConfigApp(L"add", NULL);
}

//-----------------------------------------------------------------------------
/**
	@brief Obtain port configuration information from the user and sends it to 
	       the port monitor server DLL.
	@param pszServer The name of the server, can be NULL.
	@param hWnd Parent window for dialog boxes, if NULL no dialog boxes should
	            be displayed.
	@param pszPortName The name of the port to be configured.
	@return TRUE if the port was successfully configured, FALSE otherwise.
*/
BOOL
WINAPI
ConfigurePortUI(PCWSTR pszServer, HWND hWnd, PCWSTR pszPortName)
{
	return LaunchConfigApp(L"configure", pszPortName);
}

//-----------------------------------------------------------------------------
/**
	@brief Delete a printer port.
	@param pszServer The name of the server, can be NULL.
	@param hWnd Parent window for dialog boxes, if NULL no dialog boxes should
	            be displayed.
	@param pszPortName The name of the port to be deleted.
	@return TRUE if the port was deleted successfully, FALSE otherwise.
*/
BOOL
WINAPI
DeletePortUI(PCWSTR pszServer, HWND hWnd, PCWSTR pszPortName)
{
	std::wstring printerName = ConstructXcvName(pszServer, L"XcvPort", pszPortName);

	HANDLE printerHandle = NULL;
	PRINTER_DEFAULTS printerDefaults;
	printerDefaults.pDatatype = NULL;
	printerDefaults.pDevMode = NULL;
	printerDefaults.DesiredAccess = SERVER_ACCESS_ADMINISTER;
	
	DoTraceMessage(MSG_INFO, "Establish connection to server (%S)", printerName.c_str());
	if (!OpenPrinter((LPWSTR)printerName.c_str(), &printerHandle, &printerDefaults))
		return FALSE;
		
	DWORD serverOutput;
	DWORD serverOutputMinSize;
	DWORD serverStatus;

	DoTraceMessage(MSG_INFO, "Request the server to delete port (%S)", pszPortName);
	BOOL retval = XcvData(
		printerHandle, 
		L"DeletePort", 
		(BYTE*)pszPortName, (wcslen(pszPortName) + 1) * sizeof(wchar_t),
		(BYTE*)&serverOutput,
		sizeof(serverOutput),
		&serverOutputMinSize,
		&serverStatus
	);
	
	if (!retval && (ERROR_BUSY == serverStatus))
	{
		if (hWnd)
		{
			MessageBox(
				hWnd, 
				L"This port cannot be deleted because it is in use.", 
				L"Local Port", 
				MB_OK | MB_ICONSTOP
			);
		}
		SetLastError(ERROR_CANCELLED);
		retval = FALSE;
	}
	else if (retval && (ERROR_SUCCESS != serverStatus))
	{
		SetLastError(serverStatus);
		retval = FALSE;
	}
	
	if (printerHandle)
		ClosePrinter(printerHandle);
	
	return retval;
}

MONITORUI MonitorUI = {
	sizeof(MONITORUI),
	AddPortUI,
	ConfigurePortUI,
	DeletePortUI
};

//-----------------------------------------------------------------------------
/**
	@brief Supply the print spooler with addresses of DLL functions.
*/
extern "C"
PMONITORUI
InitializePrintMonitorUI()
{
	return &MonitorUI;
}
