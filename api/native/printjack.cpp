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
#include "printjack.h"

namespace {

//-----------------------------------------------------------------------------
/**
	@brief Construct a special printer name that can be used with OpenPrinter()
	       to establish a connection to a port monitor server dll.
*/
void
ConstructXcvName(const wchar_t* portName, wchar_t* xcvName, int xcvNameMaxLen)
{
	wcscpy_s(xcvName, xcvNameMaxLen, L",XcvPort ");
	wcscat_s(xcvName, xcvNameMaxLen, portName);
}

//-----------------------------------------------------------------------------
/**
	@brief Encode a job id, parameter name and value into a single string.
	@param encodedParam Buffer to store the encoded parameter in.
	@param encodedParamMaxLen Specify the maximum number of wchar_t(s) that 
	                          can be stored in encodedParam. If this function
	                          returns true this will contain the number of
	                          wchar_t(s) actually stored in encodedParam, if
	                          the return value is false this will contain the
	                          minimum number of wchar_t(s) in encodedParam
	                          necessary to perform the encoding.
	@return true if the parameter was successfully encoded, false otherwise.
*/
bool
EncodeParam(DWORD jobId, const wchar_t* paramName, const wchar_t* paramValue, 
            wchar_t* encodedParam, size_t* encodedParamMaxLen)
{
	bool retval = true;
	size_t charsNeeded = 0;
	
	const size_t jobIdStrMaxLen = 50;
	wchar_t jobIdStr[jobIdStrMaxLen];
	_ultow_s(jobId, jobIdStr, jobIdStrMaxLen, 16);
	
	if (*encodedParamMaxLen > 0)
		encodedParam[0] = 0;
	
	charsNeeded += wcslen(jobIdStr) + 1;
	if (*encodedParamMaxLen < charsNeeded)
	{
		retval = false;
	}
	else
	{
		wcscat_s(encodedParam, *encodedParamMaxLen, jobIdStr);
		wcscat_s(encodedParam, *encodedParamMaxLen, L";");	
	}
	
	charsNeeded += wcslen(paramName) + 1;
	if (*encodedParamMaxLen < charsNeeded)
	{
		retval = false;
	}
	else
	{
		wcscat_s(encodedParam, *encodedParamMaxLen, paramName);
		wcscat_s(encodedParam, *encodedParamMaxLen, L";");
	}
	
	charsNeeded += wcslen(paramValue) + 1;
	if (*encodedParamMaxLen < charsNeeded)
	{
		retval = false;
	}
	else
	{
		wcscat_s(encodedParam, *encodedParamMaxLen, paramValue);
	}
	
	*encodedParamMaxLen = charsNeeded;
	return retval;
}

} // anonymous namespace

//-----------------------------------------------------------------------------
/**
	
*/
extern "C"
DWORD
PRINTJACKNATIVE_API
OpenPrintJackPort(const wchar_t* portName, HANDLE* portHandle)
{
	PRINTER_DEFAULTS printerDefaults;
	printerDefaults.pDatatype = NULL;
	printerDefaults.pDevMode = NULL;
	// todo: this is of some concern, I've tried other access
	// masks and this one seems to be the only one that work
	// for the OpenPrinter() call below... using this mask
	// may require admin rights which is clearly not desirable
	// for what I have in mind
	// 
	// The alternative is to use a named pipe.
	printerDefaults.DesiredAccess = SERVER_ACCESS_ADMINISTER;
	
	wchar_t xcvName[MAX_PATH];
	ConstructXcvName(portName, xcvName, MAX_PATH);
	
	*portHandle = NULL;
	if (OpenPrinter(xcvName, portHandle, &printerDefaults))
		return ERROR_SUCCESS;
	
	return GetLastError();
}

//-----------------------------------------------------------------------------
/**
	
*/
extern "C"
DWORD 
PRINTJACKNATIVE_API
SetPrintJackPortStringParam(HANDLE portHandle, DWORD jobId,
                            const wchar_t* paramName, const wchar_t* paramValue)
{
	size_t bufferSize = 0;
	EncodeParam(jobId, paramName, paramValue, NULL, &bufferSize);
	wchar_t* buffer = new wchar_t[bufferSize];
	EncodeParam(jobId, paramName, paramValue, buffer, &bufferSize);
	
	DWORD bytesNeeded = 0;
	DWORD serverStatus;
	BOOL retval = XcvData(
		portHandle, 
		L"SetParameter", 
		reinterpret_cast<BYTE*>(buffer), 
		static_cast<DWORD>(bufferSize * sizeof(wchar_t)),
		NULL, // output buffer
		0, // output buffer size
		&bytesNeeded,
		&serverStatus
	);
	delete[] buffer;
	
	if (retval)
		return serverStatus;
	else
		return GetLastError();
}

//-----------------------------------------------------------------------------
/**
	
*/
extern "C"
DWORD 
PRINTJACKNATIVE_API
ClosePrintJackPort(HANDLE portHandle)
{
	if (portHandle)
		if (ClosePrinter(portHandle))
			return ERROR_SUCCESS;
			
	return GetLastError();
}
