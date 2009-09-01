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
#include "portmonitor.h"
#include "xcvport.h"

namespace printjack {

// debugging
int XcvPort::instanceCount = 0;

namespace {

typedef DWORD (XcvPort::*XcvMethod)(
	BYTE* pInputData, DWORD cbInputData, 
	BYTE* pOutputData, DWORD cbOutputData, DWORD* pcbOutputNeeded
);

struct XCV_ENTRY
{
	const WCHAR* methodName;
	XcvMethod methodPtr;
};

XCV_ENTRY xcvMethods[] = {
	{ L"AddPort", &XcvPort::AddPort },
	{ L"DeletePort", &XcvPort::DeletePort },
	{ L"MonitorUI", &XcvPort::MonitorUI }
	//{ L"SetParameter", &XcvPort::SetParameter }
};

const int numXcvMethods = sizeof(xcvMethods) / sizeof(xcvMethods[0]);

XcvMethod
GetMethod(const WCHAR* methodName)
{
	for (int i = 0; i < numXcvMethods; ++i)
	{
		if (wcscmp(methodName, xcvMethods[i].methodName) == 0)
			return xcvMethods[i].methodPtr;
	}
	return NULL;
}

bool
PortExists(const wchar_t* portName)
{
	DWORD numBytesNeeded = 0;
	DWORD numPorts = 0;
	if (!EnumPorts(NULL, 1, NULL, 0, &numBytesNeeded, &numPorts))
		if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
			return true;

	if (0 == numBytesNeeded)
		return true;

	BYTE* portBuffer = new BYTE[numBytesNeeded];
	if (portBuffer)
	{
		if (EnumPorts(NULL, 1, portBuffer, numBytesNeeded, &numBytesNeeded, &numPorts))
		{
			PORT_INFO_1* portInfo = (PORT_INFO_1*)portBuffer;
			for (DWORD i = 0; i < numPorts; ++i)
			{
				if (_wcsicmp(portInfo[i].pName, portName) == 0)
					return true;
			}
		}
		delete[] portBuffer;
	}

	return false;
}

} // anonymous namespace

//-----------------------------------------------------------------------------
/**
	@brief Constructor.
*/
XcvPort::XcvPort(const std::wstring& name, ACCESS_MASK accessMask, 
                 PortMonitor* monitor)
: name(name),
  accessMask(accessMask),
  monitor(monitor)
{
	// debugging
	this->instanceId = ++XcvPort::instanceCount;
}

//-----------------------------------------------------------------------------
/**
	@brief Destructor.
*/
XcvPort::~XcvPort()
{
	// empty
}

//-----------------------------------------------------------------------------
/**
	@brief Add a new port that's managed by the port monitor.
	@param pInputData The name of the port to be deleted.
*/
DWORD 
XcvPort::AddPort(BYTE* pInputData, DWORD cbInputData, BYTE* pOutputData, 
                 DWORD cbOutputData, DWORD* pcbOutputNeeded)
{
	if (!(this->accessMask & SERVER_ACCESS_ADMINISTER))
		return ERROR_ACCESS_DENIED;
		
	if ((0 == cbInputData) || (NULL == pInputData))
		return ERROR_INVALID_PARAMETER;
		
	wchar_t* portName = reinterpret_cast<wchar_t*>(pInputData);
	size_t portNameLen = cbInputData / sizeof(wchar_t);
		
	// ensure the port name is null terminated
	if (0 != portName[portNameLen - 1])
		return ERROR_INVALID_PARAMETER;
	
	if (((wcslen(portName) + 1) * sizeof(wchar_t)) != cbInputData)
		return ERROR_INVALID_PARAMETER;

	if (PortExists(portName))
		return ERROR_ALREADY_EXISTS;
	else if (this->monitor->AddPort(portName))
		return ERROR_SUCCESS;

	return GetLastError();
}

//-----------------------------------------------------------------------------
/**
	@brief Delete a port that's managed by the port monitor.
	@param pInputData The name of the port to be deleted.
*/
DWORD 
XcvPort::DeletePort(BYTE* pInputData, DWORD cbInputData, BYTE* pOutputData, 
                    DWORD cbOutputData, DWORD* pcbOutputNeeded)
{
	if (!(this->accessMask & SERVER_ACCESS_ADMINISTER))
		return ERROR_ACCESS_DENIED;
		
	if ((0 == cbInputData) || (NULL == pInputData))
		return ERROR_INVALID_PARAMETER;
		
	wchar_t* portName = reinterpret_cast<wchar_t*>(pInputData);
	size_t portNameLen = cbInputData / sizeof(wchar_t);

	// ensure the port name is null terminated
	if (0 != portName[portNameLen - 1])
		return ERROR_INVALID_PARAMETER;

	if (((wcslen(portName) + 1) * sizeof(wchar_t)) != cbInputData)
		return ERROR_INVALID_PARAMETER;
		
	if (this->monitor->DeletePort(portName))
		return ERROR_SUCCESS;
	
	return ERROR_FILE_NOT_FOUND;
}

//-----------------------------------------------------------------------------
/**
	@brief Get the name of the associated port monitor UI DLL.
	@param pOutputData Buffer to store the name of the port monitor UI DLL.
	@return ERROR_SUCCESS on success, ERROR_INSUFFICIENT_BUFFER if the
	        pOutputData buffer is too small.
*/
DWORD 
XcvPort::MonitorUI(BYTE* pInputData, DWORD cbInputData, BYTE* pOutputData, 
                   DWORD cbOutputData, DWORD* pcbOutputNeeded)
{
	*pcbOutputNeeded = (wcslen(PORT_MONITOR_UI_DLL) + 1) * sizeof(wchar_t);

	if (cbOutputData < *pcbOutputNeeded)
	{
		return ERROR_INSUFFICIENT_BUFFER;
	}
	else
	{
		memcpy_s(pOutputData, cbOutputData, PORT_MONITOR_UI_DLL, *pcbOutputNeeded);
		return ERROR_SUCCESS;
	}
}

//-----------------------------------------------------------------------------
/**
	@brief Set a parameter for a specific print job.
	@param pInputData A buffer of wchar_t characters that contains a single 
	                  null-terminated string in the format
	                  "JobId;ParamName;ParamValue".
	@param cbInputData The size (in bytes) of the buffer in pInputData.
	@param pOutputData Not used.
	@param cbOutputData Not used.
	@param pcbOutputNeeded Not used.
	@return ERROR_SUCCESS on success, ERROR_INVALID_PARAMETER on failure.
*/
/*
DWORD 
XcvPort::SetParameter(BYTE* pInputData, DWORD cbInputData, 
                      BYTE* pOutputData, DWORD cbOutputData, 
                      DWORD* pcbOutputNeeded)
{
	wchar_t* input = reinterpret_cast<wchar_t*>(pInputData);
	size_t inputSizeInChars = cbInputData / sizeof(wchar_t);
	if (0 == inputSizeInChars)
		return ERROR_INVALID_PARAMETER;
	
	// make sure the string is null-terminated
	if (input[inputSizeInChars - 1] != 0)
		return ERROR_INVALID_PARAMETER;
	
	std::wstring jobIdStr;
	std::wstring paramName;
	std::wstring paramValue;
	for (size_t i = 0; i < (inputSizeInChars - 1); ++i)
	{
		if (L';' == input[i])
		{
			if (jobIdStr.empty())
			{
				jobIdStr.assign(input, i);
			}
			else if (paramName.empty())
			{
				paramName.assign(&input[jobIdStr.length() + 1], i - (jobIdStr.length() + 1));
				paramValue.assign(&input[i + 1]);
				break; // all done
			}
		}
	}
	
	DWORD jobId = wcstoul(jobIdStr.c_str(), NULL, 16);
	if ((0 == jobId) || (ULONG_MAX == jobId))
		return ERROR_INVALID_PARAMETER;
		
	this->monitor->SetJobParameter(jobId, paramName, paramValue);
	return ERROR_SUCCESS;
}
*/
//-----------------------------------------------------------------------------
/**
	@brief Maps the given method name to an XcvPort member-function pointer 
	       and calls it on the given XcvPort instance with the rest of the
	       arguments.
	@param methodName One of the following:
	                  - AddPort
	                  - DeletePort
	                  - MonitorUI
	                  - SetParameter
	                  .
	@return ERROR_INVALID_PARAMETER if no member-function matches the given
	        method name, otherwise the return value is whatever is returned
	        by the corresponding member function.
*/
DWORD
XcvPort::CallMethod(XcvPort* port, const wchar_t* methodName, BYTE* pInputData,
                    DWORD cbInputData, BYTE* pOutputData, DWORD cbOutputData,
                    DWORD* pcbOutputNeeded)
{
	XcvMethod methodPtr = GetMethod(methodName);
	if (methodPtr)
	{
		return (port->*methodPtr)(
			pInputData, cbInputData, pOutputData, cbOutputData, pcbOutputNeeded
		);
	}
	return ERROR_INVALID_PARAMETER;
}

} // namespace printjack
