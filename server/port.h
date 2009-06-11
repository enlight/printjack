#pragma once
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
#include "portconfig.h"
#include "converter.h"

namespace printjack {

class PortMonitor;
class PortMonitorRegistry;

class Port
{
public:
	Port(const WCHAR* szName, PortMonitor* portMonitor);
	~Port();
	
	const std::wstring& GetName() const;
	DWORD GetJobId() const;
	int GetPortSize(int level) const;
	LPBYTE FillPortInfo1(PORT_INFO_1* portInfo, LPBYTE endOfNextString) const;
	LPBYTE FillPortInfo2(PORT_INFO_2* portInfo, LPBYTE endOfNextString) const;
	
	BOOL StartDoc(LPWSTR pPrinterName, DWORD dwJobId, DWORD dwLevel, LPBYTE pDocInfo);
	BOOL Write(LPBYTE pBuffer, DWORD cbBuf, LPDWORD pcbWritten);
	BOOL Read(LPBYTE pBuffer, DWORD cbBuffer, LPDWORD pcbRead);
	BOOL EndDoc();

private:
	static LPWSTR PackStringAt(LPBYTE* endOfString, const WCHAR* szString);

	std::wstring name;
	PortMonitor* monitor;
	bool docStarted;
	HANDLE printerHandle;
	std::wstring printerName;
	DWORD jobId;
	PortConfig config;
	Converter* converter;
};

//-----------------------------------------------------------------------------
/**
	
*/
inline
const std::wstring&
Port::GetName() const
{
	return this->name;
}

//-----------------------------------------------------------------------------
/**
	
*/
inline
DWORD
Port::GetJobId() const
{
	return this->jobId;
}

} // namespace printjack
