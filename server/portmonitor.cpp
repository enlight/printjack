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
#include "jobparameters.h"
#include "port.h"

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
	// empty
}

//-----------------------------------------------------------------------------
/**
	
*/
PortMonitor::~PortMonitor()
{
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
}

//-----------------------------------------------------------------------------
/**
	@brief Create a new port and associate it with the monitor.
	@param portName The name to give the new port.
	@return A new port instance on success, NULL on failure.
*/
Port* 
PortMonitor::AddPort(const wchar_t* portName)
{
	std::auto_ptr<Port> port(new Port(portName, this));
	if (!this->AddPortToRegistry(portName))
		return NULL;
	this->ports.push_back(port.get());
	return port.release();
}

//-----------------------------------------------------------------------------
/**
	@brief Delete the port matching the given name.
	@return true if a port matching the given name was deleted, false otherwise.
*/
bool 
PortMonitor::DeletePort(const wchar_t* portName)
{
	std::vector<Port*>::iterator it;
	for (it = this->ports.begin(); it != this->ports.end(); ++it)
	{
		if (wcscmp((*it)->GetName().c_str(), portName) == 0)
		{
			this->RemovePortFromRegistry(portName);
			this->ports.erase(it);
			delete (*it);
			return true;
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
				this->ports.push_back(new Port(portName, this));
				
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
	std::vector<Port *>::const_iterator it;
	for (it = this->ports.begin(); it != this->ports.end(); ++it)
	{
		numBytesNeeded += (*it)->GetPortSize(level);
	}
	*bytesNeeded = numBytesNeeded;
	
	if (portsBufferSizeInBytes < numBytesNeeded)
	{
		lastError = ERROR_INSUFFICIENT_BUFFER;
	}
	else
	{
		LPBYTE endOfNextString = portsBuffer + portsBufferSizeInBytes;
		*numPortsReturned = 0;
		
		for (it = this->ports.begin(); it != this->ports.end(); ++it)
		{
			if (1 == level)
			{
				endOfNextString = (*it)->FillPortInfo1((PORT_INFO_1 *)portsBuffer, endOfNextString);
				portsBuffer += sizeof(PORT_INFO_1);
			}
			else if (2 == level)
			{
				endOfNextString = (*it)->FillPortInfo2((PORT_INFO_2 *)portsBuffer, endOfNextString);
				portsBuffer += sizeof(PORT_INFO_2);
			}
			
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

//-----------------------------------------------------------------------------
/**
	
*/
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

//-----------------------------------------------------------------------------
/**
	
*/
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

//-----------------------------------------------------------------------------
/**
	
*/
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
