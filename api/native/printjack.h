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

//
// All files within this DLL are compiled with the PRINTJACKNATIVE_EXPORTS
// symbol defined on the command line. This symbol should not be defined in 
// any project that uses this DLL.
//
// This way any other project whose source files include this file see 
// PRINTJACKNATIVE_API functions as being imported from a DLL, whereas this 
// DLL sees symbols defined with this macro as being exported.
//
#ifndef PRINTJACKNATIVE_STATIC_LIB
#ifdef PRINTJACKNATIVE_EXPORTS
#define PRINTJACKNATIVE_API __declspec(dllexport)
#else
#define PRINTJACKNATIVE_API __declspec(dllimport)
#endif
#else
#define PRINTJACKNATIVE_API
#endif

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

DWORD
PRINTJACKNATIVE_API
OpenPrintJackPort(const wchar_t* portName, HANDLE* portHandle);

DWORD 
PRINTJACKNATIVE_API
SetPrintJackPortStringParam(HANDLE portHandle, DWORD jobId,
							const wchar_t* paramName, const wchar_t* paramValue);

DWORD
PRINTJACKNATIVE_API
ClosePrintJackPort(HANDLE portHandle);

#ifdef __cplusplus							
} // extern "C"
#endif // __cplusplus
