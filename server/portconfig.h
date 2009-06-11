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

class JobParameters;
class PortMonitorRegistry;

//-----------------------------------------------------------------------------
/**
	@class PortConfig
	@brief Provides access to port configuration information stored in the
	       registry.
*/
class PortConfig
{
public:
	PortConfig(const wchar_t* portName, const PortMonitorRegistry* registry);
	
	void Load();
	void Clear();
	
	const std::wstring& GetAppPath() const;
	std::wstring GetAppArgs(const JobParameters* params) const;
	
private:
	void AddSetting(const std::wstring& settingName, DWORD settingType,
	                BYTE* settingValue, DWORD settingValueSize);

	std::wstring portName;
	const PortMonitorRegistry* registry;
	
	std::wstring appPath;
	std::wstring appArgs;
};

//-----------------------------------------------------------------------------
/**
	@brief Get the path to the application that should be launched to process 
	       print data.
*/
inline
const std::wstring& 
PortConfig::GetAppPath() const
{
	return this->appPath;
}

} // namespace printjack
