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

using namespace System;

namespace PrintJackConfigManaged {

public ref class PortAlreadyExistsException : public Exception
{
public:
	PortAlreadyExistsException(String^ portName);
};

public ref class PortInUseException : public Exception
{
public:
	PortInUseException(String^ portName);
};

} // namespace PrintJackConfigManaged
