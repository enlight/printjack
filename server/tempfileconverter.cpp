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
#include "portconfig.h"
#include "jobparameters.h"
#include "tempfileconverter.h"

namespace printjack {

//-----------------------------------------------------------------------------
/**
	
*/
TempFileConverter::TempFileConverter(const PortConfig* config, 
                                     const JobParameters* params)
: config(config),
  params(0),
  tempFileHandle(INVALID_HANDLE_VALUE)
{
	if (params)
		this->params = *params;
}

//-----------------------------------------------------------------------------
/**
	
*/
TempFileConverter::~TempFileConverter()
{
	// empty
}

//-----------------------------------------------------------------------------
/**
	
*/
bool 
TempFileConverter::Start()
{
	if (!this->CreateTempFile())
		return false;

	this->tempFileHandle = CreateFile(
		this->tempFilename.c_str(), 
		GENERIC_WRITE,
		0, // no sharing
		NULL, // security attributes
		CREATE_ALWAYS, // overwrite existing file
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	
	if (!this->tempFileHandle || (INVALID_HANDLE_VALUE == this->tempFileHandle))
		return false;
	
	return true;
}

//-----------------------------------------------------------------------------
/**
	
*/
void 
TempFileConverter::Write(LPBYTE pBuffer, DWORD cbBuf, LPDWORD pcbWritten)
{
	if (!this->tempFileHandle || 
	    (INVALID_HANDLE_VALUE == this->tempFileHandle))
	{
		//SetLastError(ERROR_INVALID_HANDLE);
		//return FALSE;
		// todo: throw exception
	}
	else
	{
		BOOL retval = WriteFile(this->tempFileHandle, pBuffer, cbBuf, pcbWritten, NULL);
		if (!retval)
		{
			//return FALSE;
			// todo: throw exception
		}
		else if (0 == *pcbWritten)
		{
			//SetLastError(ERROR_TIMEOUT);
			//return FALSE;
			// todo: throw exception
		}
	}
}

//-----------------------------------------------------------------------------
/**
	
*/
void 
TempFileConverter::End()
{
	FlushFileBuffers(this->tempFileHandle);
	CloseHandle(this->tempFileHandle);
	this->tempFileHandle = INVALID_HANDLE_VALUE;
	
	this->CreateChildProcess();
	DeleteFile(this->tempFilename.c_str());
}

//-----------------------------------------------------------------------------
/**
	
*/
bool
TempFileConverter::CreateTempFile()
{
	DWORD tempPathSize = 512; // note this is the number of elements in tempPath, not bytes!
	WCHAR tempPath[512];
	DWORD retval = GetTempPath(tempPathSize, tempPath);
	if ((retval > tempPathSize) || (retval == 0))
	{
		return false;
	}
	
	WCHAR tempFilename[MAX_PATH];
	UINT uRetVal = GetTempFileName(tempPath, L"PS", 0, tempFilename);
	if (uRetVal == 0)
	{
		return false;
	}
	
	this->tempFilename = tempFilename;
	return true;
}

//-----------------------------------------------------------------------------
/**

*/
std::wstring 
TempFileConverter::BuildCommandLine()
{
	this->params.Set(L"InputFile", this->tempFilename);
	std::wstring appArgs = this->config->GetAppArgs(&this->params);
	// extract the application name from the path
	std::wstring appName = this->config->GetAppPath();
	size_t pos = appName.find_last_of(L"\\/");
	if (std::string::npos != pos)
		appName.erase(0, pos + 1);
	
	return appName + L" " + appArgs;
}

//-----------------------------------------------------------------------------
/**
	
*/
bool 
TempFileConverter::CreateChildProcess()
{
	PROCESS_INFORMATION processInfo;
	ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));
	
	STARTUPINFO startupInfo;
	ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO); 

	std::wstring cmdLine = this->BuildCommandLine();

	BOOL processCreated = CreateProcess(
		this->config->GetAppPath().c_str(), //appPath.c_str(), 
		(LPWSTR)cmdLine.c_str(), // command line 
		NULL, // process security attributes 
		NULL, // primary thread security attributes 
		FALSE, // handles are not inherited 
		CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, // creation flags 
		NULL, // use parent's environment 
		NULL, // use parent's current directory 
		&startupInfo,
		&processInfo
	);

	if (processCreated)
	{
		// wait for child process to terminate... this is probably a bad idea
		// since it'll block and the spooler might think we've hanged, should
		// probably spawn a new thread to wait on the external process
		WaitForSingleObject(processInfo.hProcess, INFINITE);
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
		return true;
	}
	
	return false;
}

} // namespace printjack
