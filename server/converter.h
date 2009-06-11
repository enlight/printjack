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
	@class Converter
	@brief A converter transfers print data from the spooler to an 
	       application (e.g. Ghostscript) that converts it to some other 
	       format (e.g. PDF).
	
	There are various ways to transfer data from one process to another, 
	so each supported transport mechanism should have an associated Converter 
	subclass.
*/
class Converter
{
public:
	virtual ~Converter();
	
	/// called to indicate the spooler is ready to start sending data
	virtual bool Start() = 0;
	/// receive data from the spooler
	virtual void Write(BYTE* buffer, DWORD bufferSize, DWORD* bytesWritten) = 0;
	/// called to indicate the spooler has finished sending data
	virtual void End() = 0;
};

} // namespace printjack
