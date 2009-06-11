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
#include <map>

namespace printjack {

//-----------------------------------------------------------------------------
/**
	@class JobParameters
	@brief Provides access to job-specific parameters that are usually set by 
	       an external application before it starts printing.
*/
class JobParameters
{
public:
	explicit JobParameters(DWORD jobId);
	JobParameters& operator= (const JobParameters& other);
	
	DWORD GetJobId() const;
	
	void Set(const std::wstring& paramName, const std::wstring& paramValue);
	std::wstring GetString(const std::wstring& paramName) const;
	std::wstring Resolve(const std::wstring& format) const;
	
#if DBG == 1
	void Trace() const;
#endif
	
private:
	DWORD jobId;
	typedef std::map<std::wstring, std::wstring> ParamValueMap;
	ParamValueMap values;
};

//-----------------------------------------------------------------------------
/**
	
*/
inline
DWORD
JobParameters::GetJobId() const
{
	return this->jobId;
}

} // namespace printjack
