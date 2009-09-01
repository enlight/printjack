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
#include "ipcprotocol.h"
#include "pipeclient.h"
#include "loki/ScopeGuard.h"

namespace printjack {

const wchar_t* PORTS_KEY_NAME = L"Ports";

//-----------------------------------------------------------------------------
/**
	
*/
PortMonitor::PortMonitor(HINSTANCE hModule, PMONITORINIT pMonitorInit)
: moduleHandle(hModule),
  monitorInit(pMonitorInit),
  registry(pMonitorInit)
{
	WCHAR szMonitorName[MAX_PATH + 1];
	LoadString(hModule, IDS_MONITORNAME, szMonitorName, MAX_PATH);
	this->monitorName = szMonitorName;
	WCHAR szPortDesc[MAX_PATH + 1];
	LoadString(hModule, IDS_PORTDESC, szPortDesc, MAX_PATH);
	this->monitorPortDesc = szPortDesc;
}

//-----------------------------------------------------------------------------
/**
	
*/
PortMonitor::~PortMonitor()
{
	/*
	for (size_t i = 0; i < this->ports.size(); ++i)
	{
		delete this->ports[i];
	}
	this->ports.clear();
	
	for (size_t i = 0; i < this->parameters.size(); ++i)
	{
		delete this->parameters[i];
	}
	this->parameters.clear();
	*/
}

//-----------------------------------------------------------------------------
/**
	@brief Create a new port and associate it with the monitor.
	@param portName The name to give the new port.
	@return true on success, false on failure.
*/
bool
PortMonitor::AddPort(const wchar_t* portName)
{
	if (!this->AddPortToRegistry(portName))
		return false;
		
	Loki::ScopeGuard registryGuard = Loki::MakeGuard(
		&PortMonitor::RemovePortFromRegistry, *this, portName
	);
	
	AddPortIpcRequest request(portName);

	PipeClient pipeClient;
	if (!pipeClient.Send(&request))
		return false;

	AddPortIpcResponse* response = request.GetExactResponse();

	if (response->GetReturnCode())
	{
		registryGuard.Dismiss();
		this->portNames.push_back(std::wstring(portName));
		return true;
	}
	else
	{
		SetLastError(response->GetErrorCode());
		return false;
	}
}

//-----------------------------------------------------------------------------
/**
	@brief Delete the port matching the given name.
	@return true on success, false on failure.
*/
bool 
PortMonitor::DeletePort(const wchar_t* portName)
{
	std::vector<std::wstring>::iterator it;
	for (it = this->portNames.begin(); it != this->portNames.end(); ++it)
	{
		if (wcscmp(it->c_str(), portName) == 0)
		{
			if (!this->RemovePortFromRegistry(portName))
				return false;
				
			Loki::ScopeGuard registryGuard = Loki::MakeGuard(&PortMonitor::AddPortToRegistry, *this, portName);
				
			DeletePortIpcRequest request(portName);

			PipeClient pipeClient;
			if (!pipeClient.Send(&request))
				return false;

			DeletePortIpcResponse* response = request.GetExactResponse();

			if (response->GetReturnCode())
			{
				registryGuard.Dismiss();
				this->portNames.erase(it);
				return true;
			}
			else
			{
				SetLastError(response->GetErrorCode());
				return false;
			}
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
/**
	@brief Load ports associated with this monitor.
	
	Multiple ports may be associated with a monitor, the ports are stored
	under the "Ports" key in the registry.
*/
void 
PortMonitor::LoadPorts()
{
	this->criticalSection.Enter();
	
	HANDLE portsKeyHandle = NULL;
	
	DWORD retval = this->monitorInit->pMonitorReg->fpOpenKey(
		this->monitorInit->hckRegistryRoot,
		PORTS_KEY_NAME,
		KEY_READ,
		&portsKeyHandle,
		this->monitorInit->hSpooler
	);
	
	DWORD numSubKeys = 0;
	DWORD maxSubKeyNameLen = 0;
	if (ERROR_SUCCESS == retval)
	{
		retval = this->monitorInit->pMonitorReg->fpQueryInfoKey(
			portsKeyHandle,
			&numSubKeys, // number of subkeys
			&maxSubKeyNameLen, // max subkey length (excluding null)
			NULL, // number of values
			NULL, // max value name length (excluding null)
			NULL, // max data length
			NULL, // security descriptor
			NULL, // last write time
			this->monitorInit->hSpooler
		);
	}
	
	if ((ERROR_SUCCESS == retval) && (numSubKeys > 0))
	{
		DWORD portNameSize = maxSubKeyNameLen + 1;
		WCHAR* portName = new WCHAR[portNameSize];
		for (int i = 0; ERROR_SUCCESS == retval; ++i)
		{
			retval = this->monitorInit->pMonitorReg->fpEnumKey(
				portsKeyHandle,
				i,
				portName, // value name buffer
				&portNameSize, // size of value name buffer in chars
				NULL, // last write time
				this->monitorInit->hSpooler
			);
			
			if (ERROR_SUCCESS == retval)
				this->portNames.push_back(std::wstring(portName));
				
			portNameSize = maxSubKeyNameLen + 1;
		}
		delete[] portName;
	}	
	
	if (portsKeyHandle)
	{
		this->monitorInit->pMonitorReg->fpCloseKey(
			portsKeyHandle, this->monitorInit->hSpooler
		);
	}

	this->criticalSection.Leave(); // todo: use a scope guard
}

//-----------------------------------------------------------------------------
/**
	@brief Enumerate all the ports managed by this port monitor.
	@param serverName The name of the server for which ports should be
	                  enumerated, if this is NULL then the ports on the local
	                  machine are enumerated.
	@param level If 1 then PORT_INFO_1 structures will be returned, if 2 then
	             PORT_INFO_2 structures will be returned.
	@param portsBuffer The buffer that should be filled in with port info, this
	                   will consist of an array of PORT_INFO_1 or PORT_INFO_2
	                   structures followed by strings pointed to by structure
	                   members.
	@param portsBufferSizeInBytes The size of the portsBuffer in bytes.
	@param bytesNeeded Pointer to location that will receive the minimum
	                   size of the portsBuffer (in bytes) needed in order to 
	                   fit all the returned information.
	@param numPortsReturned Pointer to location that will receive the number of
	                        ports enumerated.
	@return true on success, false on failure.
	@note Only ports on the local machine are presently enumerated, regardless
	      of the value of serverName.
*/
bool 
PortMonitor::EnumPorts(const wchar_t* serverName, DWORD level, 
                       BYTE* portsBuffer, DWORD portsBufferSizeInBytes, 
                       DWORD* bytesNeeded, DWORD* numPortsReturned)
{
	DWORD lastError = 0;

	if ((level != 1) && (level != 2))
	{
		lastError = ERROR_INVALID_LEVEL;
		SetLastError(lastError);
		return false;
	}

	this->criticalSection.Enter();
	
	DWORD numBytesNeeded = 0;
	std::vector<std::wstring>::const_iterator it;
	for (it = this->portNames.begin(); it != this->portNames.end(); ++it)
	{
		numBytesNeeded += this->GetPortInfoSize((*it), level);
	}
	*bytesNeeded = numBytesNeeded;
	
	if (portsBufferSizeInBytes < numBytesNeeded)
	{
		lastError = ERROR_INSUFFICIENT_BUFFER;
	}
	else
	{
		BYTE* endOfNextString = portsBuffer + portsBufferSizeInBytes;
		*numPortsReturned = 0;
		
		for (it = this->portNames.begin(); it != this->portNames.end(); ++it)
		{
			portsBuffer += PortMonitor::FillPortInfo((*it), level, portsBuffer, endOfNextString);
			/*
			if (1 == level)
			{
				endOfNextString = PortMonitor::FillPortInfo1((*it), (PORT_INFO_1 *)portsBuffer, endOfNextString);
				portsBuffer += sizeof(PORT_INFO_1);
			}
			else if (2 == level)
			{
				endOfNextString = PortMonitor::FillPortInfo2((*it), (PORT_INFO_2 *)portsBuffer, endOfNextString);
				portsBuffer += sizeof(PORT_INFO_2);
			}
			*/
			++(*numPortsReturned);
		}
	}
	
	this->criticalSection.Leave(); // todo: use a scope guard
	
	if (lastError)
	{
		SetLastError(lastError);
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
/**
	@brief Obtain a handle to a port managed by the port monitor.
	@param portName The name of the port for which to obtain the handle.
	@param portHandle Pointer to location that will receive the port handle.
	@return true on success, false on failure.
*/
/*
bool 
PortMonitor::OpenPort(const wchar_t* portName, HANDLE* portHandle)
{
	if (!portName)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}
	
	this->criticalSection.Enter();
	
	Port *port = NULL;
	std::vector<Port *>::const_iterator it;
	for (it = this->ports.begin(); it != this->ports.end(); ++it)
	{
		if (_wcsicmp(portName, (*it)->GetName().c_str()) == 0)
		{
			port = *it;
			break;
		}
	}
	
	if (port)
		*portHandle = port;
	
	this->criticalSection.Leave();
	
	if (port)
		return true;
	else
		return false;
}
*/
//-----------------------------------------------------------------------------
/**
	
*/
/*
void 
PortMonitor::SetJobParameter(DWORD jobId, const std::wstring& paramName,
                             const std::wstring& paramValue)
{
	for (size_t i = 0; i < this->parameters.size(); ++i)
	{
		if (this->parameters[i]->GetJobId() == jobId)
		{
			this->parameters[i]->Set(paramName, paramValue);
			return;
		}
	}
	// no parameter block exists for this job, create a new one
	JobParameters* jobParams = new JobParameters(jobId);
	if (jobParams)
	{
		jobParams->Set(paramName, paramValue);
		this->parameters.push_back(jobParams);
	}
}
*/
//-----------------------------------------------------------------------------
/**
	
*/
/*
const JobParameters* 
PortMonitor::GetJobParameters(DWORD jobId)
{
	for (size_t i = 0; i < this->parameters.size(); ++i)
	{
		if (this->parameters[i]->GetJobId() == jobId)
			return this->parameters[i];
	}
	return NULL;
}
*/
//-----------------------------------------------------------------------------
/**
	
*/
/*
void 
PortMonitor::RemoveJobParameters(DWORD jobId)
{
	std::vector<JobParameters*>::iterator it;
	for (it = this->parameters.begin(); it != this->parameters.end(); ++it)
	{
		if ((*it)->GetJobId() == jobId)
		{
			this->parameters.erase(it);
			break;
		}
	}
}
*/

//-----------------------------------------------------------------------------
/**
	@brief Get the number of bytes required to store a PORT_INFO_1 or 
	       PORT_INFO_2 structure and associated strings.
*/
DWORD 
PortMonitor::GetPortInfoSize(const std::wstring& portName, DWORD level) const
{
	int numBytes = 0;

	if (1 == level)
	{
		numBytes = sizeof(PORT_INFO_1);
		numBytes += (portName.length() + 1) * sizeof(wchar_t);
	}
	else if (2 == level)
	{
		numBytes = sizeof(PORT_INFO_2);
		numBytes += (
			portName.length() + 1 +
			this->monitorName.length() + 1 +
			this->monitorPortDesc.length() + 1
		) * sizeof(wchar_t);
	}

	return numBytes;
}

//-----------------------------------------------------------------------------
/**

*/
wchar_t* 
PortMonitor::PackStringAt(BYTE** endOfPackedString, const std::wstring& stringToPack)
{
	if (!stringToPack.empty()) 
	{
		size_t stringSize = (stringToPack.length() + 1) * sizeof(wchar_t);
		wchar_t* startOfPackedString = (wchar_t*)((*endOfPackedString) - stringSize);
		wcscpy_s(startOfPackedString, stringSize, stringToPack.c_str());
		// ensure next string will be stored before this one
		(*endOfPackedString) = (BYTE*)startOfPackedString;
		return startOfPackedString;
	} 
	return NULL;
}

//-----------------------------------------------------------------------------
/**
	
*/
DWORD
PortMonitor::FillPortInfo(const std::wstring& portName, DWORD level, 
                          BYTE* portInfo, BYTE* endOfNextString) const
{
	if (1 == level)
	{
		PORT_INFO_1* portInfo1 = reinterpret_cast<PORT_INFO_1*>(portInfo);
		portInfo1->pName = PortMonitor::PackStringAt(&endOfNextString, portName);
		return sizeof(PORT_INFO_1);
	}
	else if (2 == level)
	{
		PORT_INFO_2* portInfo2 = reinterpret_cast<PORT_INFO_2*>(portInfo);
		portInfo2->pPortName = PortMonitor::PackStringAt(&endOfNextString, portName);
		portInfo2->pMonitorName = PortMonitor::PackStringAt(&endOfNextString, this->monitorName);
		portInfo2->pDescription = PortMonitor::PackStringAt(&endOfNextString, this->monitorPortDesc);
		portInfo2->fPortType = PORT_TYPE_WRITE | PORT_TYPE_READ;
		portInfo2->Reserved = 0;
		return sizeof(PORT_INFO_2);
	}
	return 0;
}

//-----------------------------------------------------------------------------
/**
	@brief Fills in a PORT_INFO_1 structure.
	@param PORT_INFO_1
	
	The storage for the PORT_INFO_1::pName string is actually allocated at the
	end of the PORT_INFO_1 structure. So the layout in memory looks something
	like:
	----------------------
	| PORT_INFO_1::pName |
	----------------------
	| pName characters   |
	----------------------
*/
/*
BYTE* 
PortMonitor::FillPortInfo1(const std::wstring& portName, PORT_INFO_1* portInfo, 
                           BYTE* endOfNextString) const
{
	portInfo->pName = PortMonitor::PackStringAt(&endOfNextString, portName);
	return endOfNextString;
}
*/
//-----------------------------------------------------------------------------
/**
	@brief Fills in a PORT_INFO_2 structure.
	
	The storage for the PORT_INFO_2 strings is actually allocated at the
	end of the PORT_INFO_2 structure. So the layout in memory looks something
	like:
	-----------------------------
	| PORT_INFO_2::pPortName    |
	| PORT_INFO_2::pMonitorName |
	| PORT_INFO_2::pDescription |
	| PORT_INFO_2::fPortType    |
	| PORT_INFO_2::Reserved     |
	-----------------------------
	| pDescription characters   |
	| pMonitorName characters   |
	| pPortName characters      |
	-----------------------------
*/
/*
BYTE* 
PortMonitor::FillPortInfo2(const std::wstring& portName, PORT_INFO_2* portInfo, 
                           BYTE* endOfNextString) const
{
	portInfo->pPortName = PortMonitor::PackStringAt(&endOfNextString, portName);
	portInfo->pMonitorName = PortMonitor::PackStringAt(&endOfNextString, this->monitorName);
	portInfo->pDescription = PortMonitor::PackStringAt(&endOfNextString, this->monitorPortDesc);
	portInfo->fPortType = PORT_TYPE_WRITE | PORT_TYPE_READ;
	portInfo->Reserved = 0;
	return endOfNextString;
}
*/
//-----------------------------------------------------------------------------
/**
	@brief Add the given port to the monitor's port list in the registry.
	@return true if the port was added to registry, false otherwise.
*/
bool
PortMonitor::AddPortToRegistry(const wchar_t* portName)
{
	if (!portName)
		return false;

	// todo: check if RevertToPrinterSelf()/ImpersonatePrinterClient() is 
	// necessary for limited user accounts... then again those use shouldn't
	// exactly be adding ports anyway!
	// maybe necessary for Win2k clusters.

	size_t portKeyPathSize = wcslen(PORTS_KEY_NAME) + 1 + wcslen(portName) + 1;
	wchar_t* portKeyPath = new wchar_t[portKeyPathSize];
	swprintf_s(portKeyPath, portKeyPathSize, L"%s\\%s", PORTS_KEY_NAME, portName);
	
	HANDLE portKeyHandle = NULL;

	DWORD retval = this->registry.CreateKey(
		portKeyPath,
		REG_OPTION_NON_VOLATILE,
		KEY_SET_VALUE,
		NULL,
		&portKeyHandle,
		NULL
	);
	
	delete[] portKeyPath;
	
	if (portKeyHandle)
		this->registry.CloseKey(portKeyHandle);
	
	return ERROR_SUCCESS == retval;
}

//-----------------------------------------------------------------------------
/**
	@brief Remove the given port from the monitor's port list in the registry.
	@return true if the port was removed from the registry, 
	        false otherwise (possibly because it wasn't found in the registry).
*/
bool
PortMonitor::RemovePortFromRegistry(const wchar_t* portName)
{
	if (!portName)
		return false;
	
	size_t portKeyPathSize = wcslen(PORTS_KEY_NAME) + 1 + wcslen(portName) + 1;
	wchar_t* portKeyPath = new wchar_t[portKeyPathSize];
	swprintf_s(portKeyPath, portKeyPathSize, L"%s\\%s", PORTS_KEY_NAME, portName);
	
	DWORD retval = this->registry.DeleteKey(portKeyPath);
	
	delete[] portKeyPath;
	
	return ERROR_SUCCESS == retval;
}

} // namespace printjack
