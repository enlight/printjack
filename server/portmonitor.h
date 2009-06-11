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
#include "portmonitorregistry.h"
#include "criticalsection.h"
#include <vector>

namespace printjack {

class Port;
class JobParameters;

//-----------------------------------------------------------------------------
/**
	@class PortMonitor
	@brief Manages PrintJack printer ports.
*/
class PortMonitor
{
public:
	PortMonitor(HINSTANCE hModule, PMONITORINIT pMonitorInit);
	~PortMonitor();
	
	HINSTANCE GetModuleHandle() const;
	const PortMonitorRegistry* GetRegistry() const;
	
	Port* AddPort(const wchar_t* portName);
	bool DeletePort(const wchar_t* portName);
	void LoadPorts();
	bool EnumPorts(const wchar_t* serverName, DWORD level, 
	               BYTE* portsBuffer, DWORD portsBufferSizeInBytes,
	               DWORD* bytesNeeded, DWORD* numPortsReturned);
	bool OpenPort(const wchar_t* portName, HANDLE* portHandle);
	
	void SetJobParameter(DWORD jobId, const std::wstring& paramName, const std::wstring& paramValue);
	const JobParameters* GetJobParameters(DWORD jobId);
	void RemoveJobParameters(DWORD jobId);
	
private:
	bool AddPortToRegistry(const wchar_t* portName);
	bool RemovePortFromRegistry(const wchar_t* portName);

	HINSTANCE moduleHandle;
	PMONITORINIT monitorInit;
	PortMonitorRegistry registry;
	CriticalSection criticalSection;
	std::vector<Port*> ports;
	std::vector<JobParameters*> parameters;
};

//-----------------------------------------------------------------------------
/**
	
*/
inline
HINSTANCE 
PortMonitor::GetModuleHandle() const
{
	return this->moduleHandle;
}

//-----------------------------------------------------------------------------
/**
	
*/
inline
const PortMonitorRegistry* 
PortMonitor::GetRegistry() const
{
	return &this->registry;
}

} // namespace printjack
