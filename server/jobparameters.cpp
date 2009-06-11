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
#if DBG == 1
#include "trace.h"
#include "jobparameters.tmh"
#endif
#include "jobparameters.h"

namespace printjack {

//-----------------------------------------------------------------------------
/**
	@brief Constructor.
*/
JobParameters::JobParameters(DWORD jobId)
: jobId(jobId)
{
	// empty
}

//-----------------------------------------------------------------------------
/**
	@brief Assignment operator.
*/
JobParameters&
JobParameters::operator= (const JobParameters& other)
{
	this->jobId = other.jobId;
	this->values = other.values;
	return *this;
}

//-----------------------------------------------------------------------------
/**
	@brief Set the value of a parameter.
	
	If the parameter was previously set it's value will be replaced.
*/
void
JobParameters::Set(const std::wstring& paramName, const std::wstring& paramValue)
{
	this->values[paramName] = paramValue;
}

//-----------------------------------------------------------------------------
/**
	@brief Get the value of a parameter as a string.
*/
std::wstring
JobParameters::GetString(const std::wstring& paramName) const
{
	ParamValueMap::const_iterator it = this->values.find(paramName);
	if (it != this->values.end())
		return it->second;
	else
		return std::wstring();
}

//-----------------------------------------------------------------------------
/**
	@brief Substitute parameter names of the form $(ParamName) in the given
	       input string with the corresponding parameter values.
	@return The result of the substitution.
*/
std::wstring
JobParameters::Resolve(const std::wstring& format) const
{
	std::wstring paramName;
	std::wstring result = format;
	ParamValueMap::const_iterator it;
	for (it = this->values.begin(); it != this->values.end(); ++it)
	{
		paramName = L"$(";
		paramName += it->first;
		paramName += L")";
		
		size_t match;
		while (std::wstring::npos != (match = result.find(paramName)))
		{
			result.replace(match, paramName.length(), it->second);
		}
	}
	return result;
}

#if DBG == 1
void
JobParameters::Trace() const
{
	ParamValueMap::const_iterator it;
	for (it = this->values.begin(); it != this->values.end(); ++it)
	{
		DoTraceMessage(
			MSG_INFO, 
			"Job: %lu, %S = %S", 
			this->jobId, it->first.c_str(), it->second.c_str()
		);
	}
}
#endif

} // namespace printjack
