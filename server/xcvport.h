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

class PortMonitor;

//-----------------------------------------------------------------------------
/**
	@class XcvPort
	@brief Processes requests sent by an external process to the port monitor 
	       via the Windows API XcvData function.

	Applications can call the XcvData function to add or delete a printer port
	that's managed by the port monitor without invoking a user interface.
	
	Applications can also use the XcvData function to provide job specific 
	parameters to the port monitor before starting to print a document,
	unfortunately that requires privileges non-administrator users don't have 
	by default.
*/
class XcvPort
{
public:
	XcvPort(const std::wstring& name, ACCESS_MASK accessMask, PortMonitor* parent);
	~XcvPort();
	
	const std::wstring& GetName() const;
	
	DWORD AddPort(
		BYTE* pInputData, DWORD cbInputData, BYTE* pOutputData, 
		DWORD cbOutputData,	DWORD* pcbOutputNeeded
	);
	DWORD DeletePort(
		BYTE* pInputData, DWORD cbInputData, BYTE* pOutputData, 
		DWORD cbOutputData,	DWORD* pcbOutputNeeded
	);
	DWORD MonitorUI(
		BYTE* pInputData, DWORD cbInputData, BYTE* pOutputData, 
		DWORD cbOutputData,	DWORD* pcbOutputNeeded
	);
	DWORD SetParameter(
		BYTE* pInputData, DWORD cbInputData, BYTE* pOutputData, 
		DWORD cbOutputData,	DWORD* pcbOutputNeeded
	);
	
	static DWORD CallMethod(
		XcvPort* port, const wchar_t* methodName, BYTE* pInputData,
	    DWORD cbInputData, BYTE* pOutputData, DWORD cbOutputData,
	    DWORD* pcbOutputNeeded
	);
	
private:
	std::wstring name;
	ACCESS_MASK accessMask;
	PortMonitor* monitor;
	
public:
	// debugging
	int instanceId;
	static int instanceCount;
};

//-----------------------------------------------------------------------------
/**
	
*/
inline
const std::wstring&
XcvPort::GetName() const
{
	return this->name;
}

} // namespace printjack
