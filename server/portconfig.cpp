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
#include "jobparameters.h"
#include "portmonitorregistry.h"
#include "portconfig.h"

namespace printjack {

//-----------------------------------------------------------------------------
/**
	
*/
PortConfig::PortConfig(const wchar_t* portName, 
                       const PortMonitorRegistry* registry)
: portName(portName),
  registry(registry)
{
	// empty
}

//-----------------------------------------------------------------------------
/**
	
*/
void 
PortConfig::AddSetting(const std::wstring& settingName, DWORD settingType,
                       BYTE* settingValue, DWORD settingValueSize)
{
	if (L"AppPath" == settingName)
	{
		if (REG_SZ == settingType)
			this->appPath.assign(reinterpret_cast<wchar_t*>(settingValue), (settingValueSize / sizeof(wchar_t)));
	}
	else if (L"AppArgs" == settingName)
	{
		if (REG_SZ == settingType)
			this->appArgs.assign(reinterpret_cast<wchar_t*>(settingValue), (settingValueSize / sizeof(wchar_t)));
	}
}

//-----------------------------------------------------------------------------
/**
	@brief Load port configuration information from the registry.
*/
void 
PortConfig::Load()
{
	std::wstring subKey = L"Ports\\";
	subKey += this->portName;
	HANDLE keyHandle = NULL;
	DWORD retval = this->registry->OpenKey(subKey.c_str(), KEY_READ, &keyHandle);
	DWORD numValues = 0;
	DWORD maxValueLen = 0;
	DWORD maxDataSize = 0;
	if (ERROR_SUCCESS == retval)
	{
		retval = this->registry->QueryInfoKey(
			keyHandle, NULL, NULL, &numValues, &maxValueLen, &maxDataSize, NULL, NULL
		);
	}
	if ((ERROR_SUCCESS == retval) && (numValues > 0))
	{
		DWORD paramNameSizeInChars = maxValueLen + 1;
		wchar_t* paramName = new wchar_t[paramNameSizeInChars];
		DWORD paramType = 0;
		BYTE* data = new BYTE[maxDataSize];
		DWORD dataSize = maxDataSize;
		for (int i = 0; ERROR_SUCCESS == retval; ++i)
		{
			
			retval = this->registry->EnumValue(
				keyHandle, i, paramName, &paramNameSizeInChars, 
				&paramType, data, &dataSize
			);
			
			if (ERROR_SUCCESS == retval)
				this->AddSetting(paramName, paramType, data, dataSize);

			dataSize = maxDataSize;
			paramNameSizeInChars = maxValueLen + 1;
		}
		delete[] data;
		delete[] paramName;
	}	
	if (keyHandle)
		this->registry->CloseKey(keyHandle);
}

//-----------------------------------------------------------------------------
/**
	@brief Clear any stored configuration information previously loaded from 
	       the registry.
*/
void 
PortConfig::Clear()
{
	this->appPath.clear();
	this->appArgs.clear();
}

//-----------------------------------------------------------------------------
/**
	@brief Get the command line arguments that should be passed to the
	       application that should be launched to process print data.
	       
	The arguments can contain parameters of the form $(ParamName), external
	applications can set the value for each parameter prior to printing.
*/
std::wstring
PortConfig::GetAppArgs(const JobParameters* params) const
{
	// fixme: it would be a good idea to check if the setting has any 
	// parameters in it, if so the parameter names should be removed if params
	// is NULL
	if (params)
		return params->Resolve(this->appArgs);
	else
		return this->appArgs;
}

} // namespace printjack
