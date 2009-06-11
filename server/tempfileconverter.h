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
#include "converter.h"
#include "jobparameters.h"

namespace printjack {

class PortConfig;

//-----------------------------------------------------------------------------
/**
	@class TempFileConverter
	@brief Transfers print data from the spooler to an application by means of
	       a temporary file.
	       
	This converter works as follows:
	- Writes print data to a temporary file. 
	- Sets the value of the $(InputFile) parameter to the location of the
	  temporary file.
	- Launches an external application and waits for it to exit.
	- Deletes the temporary file.
	.
*/
class TempFileConverter : public Converter
{
public:
	TempFileConverter(const PortConfig* config, const JobParameters* params);
	~TempFileConverter();
	
	bool Start();
	void Write(BYTE* buffer, DWORD bufferSize, DWORD* bytesWritten);
	void End();
	
private:
	bool CreateTempFile();
	std::wstring BuildCommandLine();
	bool CreateChildProcess();
	
	const PortConfig* config;
	JobParameters params;
	std::wstring tempFilename;
	std::wstring outputFilename;
	HANDLE tempFileHandle;
};

} // namespace printjack
