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
#include "stdafx.h"
#include "PortManager.h"
#include <string>
#include <map>
#include <loki/ScopeGuard.h>
#include "Exceptions.h"

namespace PrintJackConfigManaged {

#define PRINT_JACK_PORT_MONITOR_NAME L"PrintJack"

namespace {

//-----------------------------------------------------------------------------
/**
	@brief Convert a Windows error code to a string and throw it as an 
	       exception.
*/
void
ThrowWin32Error(DWORD errorCode)
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL
	);

	String^ error = gcnew String((WCHAR*)lpMsgBuf);
	LocalFree(lpMsgBuf);
	throw error;
}

//-----------------------------------------------------------------------------
/**
	@brief Convert the last Windows error to a string and throw it as an
	       exception.
*/
void
ThrowLastError()
{
	DWORD errorCode = GetLastError();
	ThrowWin32Error(errorCode);
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
ConstructXcvName(const WCHAR* serverName, const WCHAR* objectType, 
                 const WCHAR* monitorName)
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

} // namespace anonymous

//-----------------------------------------------------------------------------
/**
	
*/
void
PortManager::AddPrintJackPort(String^ portNameIn)
{
	std::wstring xcvName = ConstructXcvName(NULL, L"XcvMonitor", PRINT_JACK_PORT_MONITOR_NAME);
	
	HANDLE xcvHandle = NULL;
	PRINTER_DEFAULTS printerDefaults;
	printerDefaults.pDatatype = NULL;
	printerDefaults.pDevMode = NULL;
	printerDefaults.DesiredAccess = SERVER_ACCESS_ADMINISTER;
	
	if (!OpenPrinter((LPWSTR)xcvName.c_str(), &xcvHandle, &printerDefaults))
		ThrowLastError();
	
	// make sure the handle is always automatically released
	Loki::ScopeGuard xcvHandleGuard = Loki::MakeGuard(ClosePrinter, xcvHandle);
	
	pin_ptr<const wchar_t> portName = PtrToStringChars(portNameIn);
	
	DWORD serverOutput;
	DWORD serverOutputMinSize;
	DWORD serverStatus;
	
	BOOL retval = XcvData(
		xcvHandle, 
		L"AddPort", 
		(PBYTE)portName, (wcslen(portName) + 1) * sizeof(WCHAR),
		(PBYTE)&serverOutput,
		sizeof(serverOutput),
		&serverOutputMinSize,
		&serverStatus
	);
	
	if (retval)
	{
		switch (serverStatus)
		{
			case ERROR_SUCCESS:
				break; // all is well
				
			case ERROR_ALREADY_EXISTS:
				throw gcnew PortAlreadyExistsException(portNameIn);
				
			default:
				ThrowWin32Error(serverStatus);
				break;
		}
	}
	else
	{
		ThrowLastError();
	}
}

//-----------------------------------------------------------------------------
/**
	
*/
void 
PortManager::DeletePrintJackPort(String^ portNameIn)
{
	pin_ptr<const wchar_t> portName = PtrToStringChars(portNameIn);
	std::wstring xcvName = ConstructXcvName(NULL, L"XcvPort", portName);

	HANDLE xcvHandle = NULL;
	PRINTER_DEFAULTS printerDefaults;
	printerDefaults.pDatatype = NULL;
	printerDefaults.pDevMode = NULL;
	printerDefaults.DesiredAccess = SERVER_ACCESS_ADMINISTER;
	
	if (!OpenPrinter((LPWSTR)xcvName.c_str(), &xcvHandle, &printerDefaults))
		ThrowLastError();
	
	// make sure the handle is always automatically released
	Loki::ScopeGuard xcvHandleGuard = Loki::MakeGuard(ClosePrinter, xcvHandle);
		
	DWORD serverOutput;
	DWORD serverOutputMinSize;
	DWORD serverStatus;

	BOOL retval = XcvData(
		xcvHandle, 
		L"DeletePort", 
		(PBYTE)portName, (wcslen(portName) + 1) * sizeof(WCHAR),
		(PBYTE)&serverOutput,
		sizeof(serverOutput),
		&serverOutputMinSize,
		&serverStatus
	);
	
	if (retval)
	{
		if (ERROR_SUCCESS == serverStatus)
		{
			// all good
		}
		else
		{
			ThrowWin32Error(serverStatus);
		}
	}
	else
	{
		if (ERROR_BUSY == serverStatus)
		{
			throw gcnew PortInUseException(portNameIn);
		}
		else
		{
			ThrowLastError();
		}
	}
}

//-----------------------------------------------------------------------------
/**
	@brief Get a list of printer port names on the local machine.
*/
List<String^>^ 
PortManager::GetPortNames()
{
	// figure out how large the buffer should be to fit all the port names
	DWORD numBytesNeeded = 0;
	DWORD numPorts = 0;
	if (!EnumPorts(NULL, 1, NULL, 0, &numBytesNeeded, &numPorts))
		if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
			return nullptr;

	if (0 == numBytesNeeded)
		return nullptr;
		
	// enumerate all the ports on the local machine
	BYTE* portBuffer = new BYTE[numBytesNeeded];
	if (portBuffer)
	{
		List<String^>^ portNames = gcnew List<String^>();
		if (EnumPorts(NULL, 1, portBuffer, numBytesNeeded, &numBytesNeeded, &numPorts))
		{
			PORT_INFO_1* portInfoBuffer = (PORT_INFO_1*)portBuffer;
			for (DWORD i = 0; i < numPorts; ++i)
			{
				portNames->Add(gcnew String(portInfoBuffer[i].pName));
			}
		}
		delete[] portBuffer;
		return portNames;
	}
	return nullptr;
}

//-----------------------------------------------------------------------------
/**
	@brief Get a list of PortInfo objects representing all the printer ports on
	       the local machine.
*/
List<PortInfo^>^
PortManager::GetPorts()
{
	// figure out how large the buffer should be to fit all the PORT_INFO_2
	// structures
	DWORD numBytesNeeded = 0;
	DWORD numPorts = 0;
	if (!EnumPorts(NULL, 2, NULL, 0, &numBytesNeeded, &numPorts))
		if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
			return nullptr;
	
	if (0 == numBytesNeeded)
		return nullptr;
	
	typedef std::pair<std::wstring, PORT_INFO_2*> PortNameToPortInfoPair;
	typedef std::map<std::wstring, PORT_INFO_2*> PortNameToPortInfoMap;
	
	// obtain info about all the ports on the local machine
	PortNameToPortInfoMap portNameToPortInfoMap;
	BYTE* portBuffer = new BYTE[numBytesNeeded];
	if (portBuffer)
	{
		if (EnumPorts(NULL, 2, portBuffer, numBytesNeeded, &numBytesNeeded, &numPorts))
		{
			PORT_INFO_2* portInfo2Buffer = (PORT_INFO_2*)portBuffer;
			for (DWORD i = 0; i < numPorts; ++i)
			{
				portNameToPortInfoMap.insert(PortNameToPortInfoPair(portInfo2Buffer[i].pPortName, &portInfo2Buffer[i]));
			}
		}
		else
		{
			delete[] portBuffer;
			return nullptr;
		}
	}
	
	// figure out how large the buffer should be to fit all the PORT_INFO_2
	// structures
	numBytesNeeded = 0;
	DWORD numPrinters = 0;
	if (!EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 5, NULL, 0, &numBytesNeeded, &numPrinters))
		if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
			return nullptr;

	if (0 == numBytesNeeded)
		return nullptr;
	
	typedef std::pair<std::wstring, PRINTER_INFO_5*> PortNameToPrinterInfoPair;
	typedef std::map<std::wstring, PRINTER_INFO_5*> PortNameToPrinterInfoMap;
	
	// obtain info about all the printers on the local machine
	PortNameToPrinterInfoMap portNameToPrinterInfoMap;
	BYTE* printerBuffer = new BYTE[numBytesNeeded];
	if (printerBuffer)
	{
		// only look for local printers
		if (EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 5, printerBuffer, numBytesNeeded, &numBytesNeeded, &numPrinters))
		{
			PRINTER_INFO_5* printerInfo5Buffer = (PRINTER_INFO_5*)printerBuffer;
			for (DWORD i = 0; i < numPrinters; ++i)
			{
				portNameToPrinterInfoMap.insert(PortNameToPrinterInfoPair(printerInfo5Buffer[i].pPortName, &printerInfo5Buffer[i]));
			}
		}
		else
		{
			delete[] printerBuffer;
			delete[] portBuffer;
			return nullptr;
		}
	}
	
	// combine the port and printer info
	List<PortInfo^>^ ports = gcnew List<PortInfo^>(numPorts);
	PortNameToPortInfoMap::const_iterator portIt;
	for (portIt = portNameToPortInfoMap.begin(); portIt != portNameToPortInfoMap.end(); ++portIt)
	{
		PortInfo^ port = gcnew PortInfo();
		port->PortName = gcnew String(portIt->first.c_str());
		port->PortDescription = gcnew String(portIt->second->pDescription);
		port->MonitorName = gcnew String(portIt->second->pMonitorName);
		PortNameToPrinterInfoMap::const_iterator printerIt = portNameToPrinterInfoMap.find(portIt->first);
		if (printerIt != portNameToPrinterInfoMap.end())
			port->PrinterName = gcnew String(printerIt->second->pPrinterName);
		else
			port->PrinterName = String::Empty;
		ports->Add(port);
	}
	
	delete[] printerBuffer;
	delete[] portBuffer;
	
	return ports;
}

} // namespace PrintJackConfigManaged
