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

//-----------------------------------------------------------------------------
/**
	@file trace.h
	@brief Bits and pieces to enable WPP Software Tracing for the port monitor.
	
	Note: In order to use PDBs built by the WinDDK in TraceView.exe you need to
	make sure TraceView uses the dbghelp.dll that comes with the DDK (older
	version lack the required functionality). The easiest way to do that is
	simply to copy the dbghelp.dll from the DDK to the directroy TraceView.exe
	lives in.
*/

#define WPP_CONTROL_GUIDS \
	WPP_DEFINE_CONTROL_GUID(PDFPortMonitorGuid,(BF20E05B,D044,418B,A018,129433B8058F), \
		WPP_DEFINE_BIT(MSG_INFO) \
		WPP_DEFINE_BIT(MSG_ERROR))
