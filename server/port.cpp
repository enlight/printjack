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
#include "port.h"
#include "portmonitor.h"
#include "tempfileconverter.h"

namespace printjack {

//-----------------------------------------------------------------------------
/**
	
*/
Port::Port(const WCHAR* szName, PortMonitor* portMonitor)
: name(szName),
  monitor(portMonitor),
  docStarted(false),
  config(szName, portMonitor->GetRegistry()),
  converter(NULL)
{
	// empty
}

//-----------------------------------------------------------------------------
/**
	
*/
Port::~Port()
{
	if (this->converter)
	{
		delete this->converter;
		this->converter = NULL;
	}
}

//-----------------------------------------------------------------------------
/**
	
*/
int
Port::GetPortSize(int level) const
{
	int numBytes = 0;
	
	if (1 == level)
	{
		numBytes = sizeof(PORT_INFO_1);
		numBytes += this->name.length() * sizeof(WCHAR) + sizeof(WCHAR);
	}
	else if (2 == level)
	{
		WCHAR szMonitorName[MAX_PATH + 1];
		LoadString(this->monitor->GetModuleHandle(), IDS_MONITORNAME, szMonitorName, MAX_PATH);
		WCHAR szPortDesc[MAX_PATH + 1];
		LoadString(this->monitor->GetModuleHandle(), IDS_PORTDESC, szPortDesc, MAX_PATH);
		numBytes = sizeof(PORT_INFO_2);
		numBytes += (
			this->name.length() + 1 +
			wcslen(szMonitorName) + 1 +
			wcslen(szPortDesc) + 1
		) * sizeof(WCHAR);
	}

	return numBytes;
}

//-----------------------------------------------------------------------------
/**
	
*/
LPBYTE 
Port::FillPortInfo1(PORT_INFO_1* portInfo, LPBYTE endOfNextString) const
{
	portInfo->pName = Port::PackStringAt(&endOfNextString, this->name.c_str());
	return endOfNextString;
}

//-----------------------------------------------------------------------------
/**
	
*/
LPBYTE 
Port::FillPortInfo2(PORT_INFO_2* portInfo, LPBYTE endOfNextString) const
{
	portInfo->pPortName = Port::PackStringAt(&endOfNextString, this->name.c_str());
	WCHAR szMonitorName[MAX_PATH + 1];
	LoadString(this->monitor->GetModuleHandle(), IDS_MONITORNAME, szMonitorName, MAX_PATH);
	portInfo->pMonitorName = Port::PackStringAt(&endOfNextString, szMonitorName);
	WCHAR szPortDesc[MAX_PATH + 1];
	LoadString(this->monitor->GetModuleHandle(), IDS_PORTDESC, szPortDesc, MAX_PATH);
	portInfo->pDescription = Port::PackStringAt(&endOfNextString, szPortDesc);
	portInfo->fPortType = PORT_TYPE_WRITE | PORT_TYPE_READ;
	portInfo->Reserved = 0;
	
	return endOfNextString;
}

//-----------------------------------------------------------------------------
/**
	
*/
BOOL 
Port::StartDoc(LPWSTR pPrinterName, DWORD dwJobId, DWORD dwLevel, LPBYTE pDocInfo)
{
	if (this->docStarted)
		return TRUE;
		
	this->docStarted = true;
	
	this->printerHandle = NULL;
	this->printerName = pPrinterName;
	
	if (OpenPrinter(pPrinterName, &this->printerHandle, NULL))
	{
		this->jobId = dwJobId;
		// load port configuration info from the registry
		this->config.Load();
		const JobParameters* params = this->monitor->GetJobParameters(dwJobId);
		#if DBG == 1
		if (params)
			params->Trace();
		#endif
		// create the appropriate converter
		// if (this->config.GetConverterType() == ConverterType::TempFile)
		    this->converter = new TempFileConverter(&this->config, params);
		// else if (this->config.GetConverterType() == ConverterType::Pipe)
		//    this->converter = new PipeConverter(this->config, this->params);
		
		// for testing only!
		//this->params.Set(L"OutputFile", L"D:\\TempTestDoc.pdf");
		
		if (this->converter->Start())
			return TRUE;
	}
	
	this->docStarted = false;
	if (this->printerHandle)
	{
		ClosePrinter(this->printerHandle);
		this->printerHandle = INVALID_HANDLE_VALUE;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
/**
	
*/
BOOL 
Port::Write(LPBYTE pBuffer, DWORD cbBuf, LPDWORD pcbWritten)
{
	this->converter->Write(pBuffer, cbBuf, pcbWritten);
	/*
	if (!this->fileHandle || (INVALID_HANDLE_VALUE == this->fileHandle))
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}
	else
	{
		BOOL retval = WriteFile(this->fileHandle, pBuffer, cbBuf, pcbWritten, NULL);
		if (!retval)
		{
			return FALSE;
		}
		else if (0 == *pcbWritten)
		{
			SetLastError(ERROR_TIMEOUT);
			return FALSE;
		}
	}
	*/
	return TRUE;
}

//-----------------------------------------------------------------------------
/**
	
*/
BOOL 
Port::Read(LPBYTE pBuffer, DWORD cbBuffer, LPDWORD pcbRead)
{
	// we don't support reading!
	SetLastError(ERROR_INVALID_HANDLE);
	return FALSE;
}

//-----------------------------------------------------------------------------
/**
	
*/
BOOL 
Port::EndDoc()
{
	if (!this->docStarted)
		return TRUE;
	
	this->converter->End();
	delete this->converter;
	this->converter = NULL;
	
	this->config.Clear();
	this->monitor->RemoveJobParameters(this->jobId);
	
	// fixme: check for JOB_STATUS_DELETING or JOB_STATUS_RESTART
	SetJob(this->printerHandle, this->jobId, 0, NULL, JOB_CONTROL_SENT_TO_PRINTER);
	
	ClosePrinter(this->printerHandle);
	
	this->docStarted = false;
	
	return TRUE;
}

//-----------------------------------------------------------------------------
/**

*/
LPWSTR
Port::PackStringAt(LPBYTE* endOfString, const WCHAR* szString)
{
	if (szString) 
	{
		size_t stringSize = wcslen(szString) * sizeof(WCHAR) + sizeof(WCHAR);
		LPWSTR startOfString = (LPWSTR)((*endOfString) - stringSize);
		wcscpy_s(startOfString, stringSize, szString);
		// ensure next string will be stored before this one
		(*endOfString) = (LPBYTE)startOfString;
		return startOfString;
	} 
	return NULL;
}

} // namespace printjack
