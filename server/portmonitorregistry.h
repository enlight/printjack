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
namespace printjack {

//-----------------------------------------------------------------------------
/**
	@class PortMonitorRegistry
	@brief Wrapper for the port monitor registry functions in MONITORREG.
*/
class PortMonitorRegistry
{
public:
	PortMonitorRegistry(PMONITORINIT monitorInit);
	
	LONG CreateKey(const wchar_t* subKey, DWORD options, REGSAM desired, 
	               PSECURITY_ATTRIBUTES securityAttributes, HANDLE* result,
	               DWORD* disposition) const;
	LONG OpenKey(const wchar_t* subKey, REGSAM desired, HANDLE* result) const;
	LONG CloseKey(HANDLE key) const;
	LONG DeleteKey(const wchar_t* subKey) const;
	LONG EnumKey(HANDLE key, DWORD index, wchar_t* name, DWORD* nameSizeInChars,
	             PFILETIME lastWriteTime) const;
	LONG QueryInfoKey(HANDLE key, DWORD* numSubKeys, DWORD* maxSubKeyLen,
	                  DWORD* numValues, DWORD* maxValueLen,
	                  DWORD* maxDataSize, DWORD* securityDescriptor, 
	                  PFILETIME lastWriteTime) const;
	LONG SetValue(HANDLE key, const wchar_t* valueName, DWORD valueType,
	              const BYTE* data, DWORD dataSize) const;
	LONG DeleteValue(HANDLE key, const wchar_t* valueName) const;
	LONG EnumValue(HANDLE key, DWORD index, wchar_t* valueName, 
	               DWORD* valueSizeInChars, DWORD* valueType,
	               BYTE* data, DWORD* dataSize) const;
	LONG QueryValue(HANDLE key, const wchar_t* valueName, DWORD* valueType,
	                BYTE* data, DWORD* dataSize) const;
	               
private:
	PMONITORINIT monitorInit;
};

//-----------------------------------------------------------------------------
/**
	
*/
inline
PortMonitorRegistry::PortMonitorRegistry(PMONITORINIT monitorInit)
: monitorInit(monitorInit)
{
	// empty
}

//-----------------------------------------------------------------------------
/**
	
*/
inline
LONG
PortMonitorRegistry::CreateKey(const wchar_t* subKey, DWORD options, 
                               REGSAM desired, 
                               PSECURITY_ATTRIBUTES securityAttributes, 
                               HANDLE* result, DWORD* disposition)
const
{
	return this->monitorInit->pMonitorReg->fpCreateKey(
		this->monitorInit->hckRegistryRoot,
		subKey, options, desired, securityAttributes, result, disposition,
		this->monitorInit->hSpooler
	);
}

//-----------------------------------------------------------------------------
/**
	
*/
inline
LONG 
PortMonitorRegistry::OpenKey(const wchar_t* subKey, REGSAM desired, 
                             HANDLE* result)
const
{
	return this->monitorInit->pMonitorReg->fpOpenKey(
		this->monitorInit->hckRegistryRoot,
		subKey, desired, result,
		this->monitorInit->hSpooler
	);
}

//-----------------------------------------------------------------------------
/**
	
*/
inline
LONG 
PortMonitorRegistry::CloseKey(HANDLE key)
const
{
	return this->monitorInit->pMonitorReg->fpCloseKey(
		key,
		this->monitorInit->hSpooler
	);
}

//-----------------------------------------------------------------------------
/**
	
*/
inline
LONG 
PortMonitorRegistry::DeleteKey(const wchar_t* subKey)
const
{
	return this->monitorInit->pMonitorReg->fpDeleteKey(
		this->monitorInit->hckRegistryRoot,
		subKey,
		this->monitorInit->hSpooler
	);
}

//-----------------------------------------------------------------------------
/**
	
*/
inline
LONG 
PortMonitorRegistry::EnumKey(HANDLE key, DWORD index, wchar_t* name, 
                             DWORD* nameSizeInChars, PFILETIME lastWriteTime) 
const
{
	return this->monitorInit->pMonitorReg->fpEnumKey(
		key, index, name, nameSizeInChars, lastWriteTime,
		this->monitorInit->hSpooler
	);
}

//-----------------------------------------------------------------------------
/**
	
*/
inline
LONG 
PortMonitorRegistry::QueryInfoKey(HANDLE key, DWORD* numSubKeys, 
                                  DWORD* maxSubKeyLen, DWORD* numValues, 
                                  DWORD* maxValueLen, DWORD* maxDataSize, 
                                  DWORD* securityDescriptor, 
                                  PFILETIME lastWriteTime)
const
{
	return this->monitorInit->pMonitorReg->fpQueryInfoKey(
		key, numSubKeys, maxSubKeyLen, numValues, maxValueLen, maxDataSize, 
		securityDescriptor, lastWriteTime,
		this->monitorInit->hSpooler
	);
}

//-----------------------------------------------------------------------------
/**
	
*/
inline
LONG 
PortMonitorRegistry::SetValue(HANDLE key, 
                              const wchar_t* valueName, DWORD valueType,
                              const BYTE* data, DWORD dataSize)
const
{
	return this->monitorInit->pMonitorReg->fpSetValue(
		key, valueName, valueType, data, dataSize,
		this->monitorInit->hSpooler
	);
}

//-----------------------------------------------------------------------------
/**
	
*/
inline
LONG 
PortMonitorRegistry::DeleteValue(HANDLE key, const wchar_t* valueName)
const
{
	return this->monitorInit->pMonitorReg->fpDeleteValue(
		key, valueName,
		this->monitorInit->hSpooler
	);
}

//-----------------------------------------------------------------------------
/**
	
*/
inline
LONG 
PortMonitorRegistry::EnumValue(HANDLE key, DWORD index, wchar_t* valueName, 
                               DWORD* valueSizeInChars, DWORD* valueType,
                               BYTE* data, DWORD* dataSize)
const
{
	return this->monitorInit->pMonitorReg->fpEnumValue(
		key, index, valueName, valueSizeInChars, valueType, data, dataSize,
		this->monitorInit->hSpooler
	);
}

//-----------------------------------------------------------------------------
/**
	
*/
inline
LONG 
PortMonitorRegistry::QueryValue(HANDLE key, const wchar_t* valueName, 
                                DWORD* valueType, BYTE* data, DWORD* dataSize)
const
{
	return this->monitorInit->pMonitorReg->fpQueryValue(
		key, valueName, valueType, data, dataSize,
		this->monitorInit->hSpooler
	);
}

} // namespace printjack
