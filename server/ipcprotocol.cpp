#include "precompiled.h"
#include "ipcprotocol.h"

namespace printjack {

//-----------------------------------------------------------------------------
/**
	
*/
IpcRequest::~IpcRequest()
{
	// empty
}

//-----------------------------------------------------------------------------
/**
	
*/
IpcResponse::~IpcResponse()
{
	// empty
}

//-----------------------------------------------------------------------------
/**

*/
struct AddPortIpcRequest::DataBlock
{
	IpcRequestDataBlockHeader header;
	wchar_t portName[MAX_PATH];
};

DefinePrintJackIpcRequest(AddPortIpcRequest)

//-----------------------------------------------------------------------------
/**

*/
AddPortIpcRequest::AddPortIpcRequest(const wchar_t* portName)
: dataOwner(true)
{
	this->data = new AddPortIpcRequest::DataBlock();
	this->data->header.reqType = IpcRequestType::AddMonitorPort;
	wcscpy_s(this->data->portName, sizeof(this->data->portName), portName);
}

//-----------------------------------------------------------------------------
/**

*/
AddPortIpcRequest::AddPortIpcRequest(BYTE* buffer)
: dataOwner(false)
{
	this->data = reinterpret_cast<AddPortIpcRequest::DataBlock*>(buffer);
}

//-----------------------------------------------------------------------------
/**
	
*/
AddPortIpcRequest::~AddPortIpcRequest()
{
	if (this->dataOwner && this->data)
	{
		delete this->data;
		this->data = NULL;
	}
}

//-----------------------------------------------------------------------------
/**
	
*/
AddPortIpcResponse*
AddPortIpcRequest::GetExactResponse()
{
	if (!this->response)
		this->response = new AddPortIpcResponse();

	return this->response;
}

//-----------------------------------------------------------------------------
/**
	
*/
const wchar_t* 
AddPortIpcRequest::GetPortName()
{
	return this->data->portName;
}

//-----------------------------------------------------------------------------
/**

*/
struct AddPortIpcResponse::DataBlock
{
	IpcResponseDataBlockHeader header;
};

DefinePrintJackIpcResponse(AddPortIpcResponse)

//-----------------------------------------------------------------------------
/**

*/
AddPortIpcResponse::AddPortIpcResponse()
: dataOwner(true)
{
	this->data = new AddPortIpcResponse::DataBlock();
}

//-----------------------------------------------------------------------------
/**

*/
AddPortIpcResponse::AddPortIpcResponse(BYTE* buffer, DWORD returnCode, DWORD errorCode)
: dataOwner(false)
{
	this->data = reinterpret_cast<AddPortIpcResponse::DataBlock*>(buffer);
	this->data->header.returnCode = returnCode;
	this->data->header.errorCode = errorCode;
}

//-----------------------------------------------------------------------------
/**

*/
AddPortIpcResponse::~AddPortIpcResponse()
{
	if (this->dataOwner && this->data)
	{
		delete this->data;
		this->data = NULL;
	}
}

//-----------------------------------------------------------------------------
/**

*/
struct DeletePortIpcRequest::DataBlock
{
	IpcRequestDataBlockHeader header;
	wchar_t portName[MAX_PATH];
};

DefinePrintJackIpcRequest(DeletePortIpcRequest)

//-----------------------------------------------------------------------------
/**

*/
DeletePortIpcRequest::DeletePortIpcRequest(const wchar_t* portName)
: dataOwner(true)
{
	this->data = new DeletePortIpcRequest::DataBlock();
	this->data->header.reqType = IpcRequestType::DeleteMonitorPort;
	wcscpy_s(this->data->portName, sizeof(this->data->portName), portName);
}

//-----------------------------------------------------------------------------
/**

*/
DeletePortIpcRequest::DeletePortIpcRequest(BYTE* buffer)
: dataOwner(false)
{
	this->data = reinterpret_cast<DeletePortIpcRequest::DataBlock*>(buffer);
}

//-----------------------------------------------------------------------------
/**

*/
DeletePortIpcRequest::~DeletePortIpcRequest()
{
	if (this->dataOwner && this->data)
	{
		delete this->data;
		this->data = NULL;
	}
}

//-----------------------------------------------------------------------------
/**

*/
DeletePortIpcResponse*
DeletePortIpcRequest::GetExactResponse()
{
	if (!this->response)
		this->response = new DeletePortIpcResponse();

	return this->response;
}

//-----------------------------------------------------------------------------
/**

*/
const wchar_t* 
DeletePortIpcRequest::GetPortName()
{
	return this->data->portName;
}

//-----------------------------------------------------------------------------
/**

*/
struct DeletePortIpcResponse::DataBlock
{
	IpcResponseDataBlockHeader header;
};

DefinePrintJackIpcResponse(DeletePortIpcResponse)

//-----------------------------------------------------------------------------
/**

*/
DeletePortIpcResponse::DeletePortIpcResponse()
: dataOwner(true)
{
	this->data = new DeletePortIpcResponse::DataBlock();
}

//-----------------------------------------------------------------------------
/**

*/
DeletePortIpcResponse::DeletePortIpcResponse(BYTE* buffer, DWORD returnCode, DWORD errorCode)
: dataOwner(false)
{
	this->data = reinterpret_cast<DeletePortIpcResponse::DataBlock*>(buffer);
	this->data->header.returnCode = returnCode;
	this->data->header.errorCode = errorCode;
}

//-----------------------------------------------------------------------------
/**

*/
DeletePortIpcResponse::~DeletePortIpcResponse()
{
	if (this->dataOwner && this->data)
	{
		delete this->data;
		this->data = NULL;
	}
}

//-----------------------------------------------------------------------------
/**
	
*/
/*
struct EnumPortsIpcResponse::DataBlock
{
	IpcResponseDataBlockHeader header;
	DWORD numBytesNeededForPorts;
	DWORD numPorts;
};

DefinePrintJackIpcGetDataSimple(EnumPortsIpcResponse)
*/
//-----------------------------------------------------------------------------
/**
	
*/
/*
EnumPortsIpcResponse::EnumPortsIpcResponse(void* portsData, DWORD portsDataSize)
: portsData(portsData), portsDataSize(portsDataSize)
{
	this->data = new EnumPortsIpcResponse::DataBlock();
	ZeroMemory(this->data, sizeof(EnumPortsIpcResponse::DataBlock));
}
*/
//-----------------------------------------------------------------------------
/**
	
*/
/*
EnumPortsIpcResponse::~EnumPortsIpcResponse()
{
	if (this->data)
	{
		delete this->data;
		this->data = NULL;
	}
}
*/
//-----------------------------------------------------------------------------
/**
	
*/	
/*
bool
EnumPortsIpcResponse::AllocateExtraData()
{
	// no allocation is necessary because the extra memory was already
	// allocated outside of this instance
	return this->portsData && this->portsDataSize;
}
*/
//-----------------------------------------------------------------------------
/**
	
*/
/*
void*
EnumPortsIpcResponse::GetExtraData()
{
	return this->portsData;
}
*/
//-----------------------------------------------------------------------------
/**
	
*/
/*
DWORD 
EnumPortsIpcResponse::GetExtraDataSize()
{
	return this->portsDataSize;
}
*/
//-----------------------------------------------------------------------------
/**
	
*/
/*
DWORD 
EnumPortsIpcResponse::GetNumBytesNeeded()
{
	return this->data->numBytesNeededForPorts;	
}
*/
//-----------------------------------------------------------------------------
/**
	
*/
/*
DWORD
EnumPortsIpcResponse::GetNumPorts()
{
	return this->data->numPorts;
}
*/
//-----------------------------------------------------------------------------
/**
	
*/
/*
struct EnumPortsIpcRequest::DataBlock
{
	IpcRequestDataBlockHeader header;
	DWORD level;
};

DefinePrintJackIpcGetDataSimple(EnumPortsIpcRequest)
*/
//-----------------------------------------------------------------------------
/**
	
*/
/*
EnumPortsIpcRequest::EnumPortsIpcRequest(DWORD level, BYTE* portsBuffer, 
                                         DWORD portsBufferSize)
: portsBuffer(portsBuffer), portsBufferSize(portsBufferSize)
{
	this->data = new EnumPortsIpcRequest::DataBlock();
	this->data->header->reqType = IpcRequestType::EnumPortsRequestType;
	this->data->level = level;
}
*/
//-----------------------------------------------------------------------------
/**
	
*/
/*
EnumPortsIpcRequest::~EnumPortsIpcRequest()
{
	if (this->data)
	{
		delete this->data;
		this->data = NULL;
	}
}
*/
//-----------------------------------------------------------------------------
/**
	
*/
/*
IpcResponse*
EnumPortsIpcRequest::GetResponse()
{
	if (!this->response)
		this->response = new EnumPortsIpcResponse(this->portsBuffer, this->portsBufferSize);
	
	return this->response;
}
*/

//-----------------------------------------------------------------------------
/**

*/
struct OpenPortIpcRequest::DataBlock
{
	IpcRequestDataBlockHeader header;
	wchar_t portName[MAX_PATH];
};

DefinePrintJackIpcRequest(OpenPortIpcRequest)

//-----------------------------------------------------------------------------
/**

*/
OpenPortIpcRequest::OpenPortIpcRequest(const wchar_t* portName)
: dataOwner(true)
{
	this->data = new OpenPortIpcRequest::DataBlock();
	ZeroMemory(this->data, sizeof(OpenPortIpcRequest::DataBlock));
	this->data->header.reqType = IpcRequestType::OpenMonitorPort;
	wcscpy_s(this->data->portName, sizeof(this->data->portName), portName);
}

//-----------------------------------------------------------------------------
/**

*/
OpenPortIpcRequest::OpenPortIpcRequest(BYTE* buffer)
: dataOwner(false)
{
	this->data = reinterpret_cast<OpenPortIpcRequest::DataBlock*>(buffer);
}

//-----------------------------------------------------------------------------
/**

*/
OpenPortIpcRequest::~OpenPortIpcRequest()
{
	if (this->dataOwner && this->data)
	{
		delete this->data;
		this->data = NULL;
	}
}

//-----------------------------------------------------------------------------
/**

*/
OpenPortIpcResponse* 
OpenPortIpcRequest::GetExactResponse()
{
	if (!this->response)
		this->response = new OpenPortIpcResponse();

	return this->response;
}

//-----------------------------------------------------------------------------
/**

*/
const wchar_t* 
OpenPortIpcRequest::GetPortName()
{
	return this->data->portName;
}

//-----------------------------------------------------------------------------
/**
	
*/
struct OpenPortIpcResponse::DataBlock
{
	IpcResponseDataBlockHeader header;
	int portHandle;
};

DefinePrintJackIpcResponse(OpenPortIpcResponse)

//-----------------------------------------------------------------------------
/**
	
*/
OpenPortIpcResponse::OpenPortIpcResponse()
: dataOwner(true)
{
	this->data = new OpenPortIpcResponse::DataBlock();
	ZeroMemory(this->data, sizeof(OpenPortIpcResponse::DataBlock));
}

//-----------------------------------------------------------------------------
/**
	
*/
OpenPortIpcResponse::OpenPortIpcResponse(BYTE* buffer, DWORD returnCode, DWORD errorCode)
: dataOwner(false)
{
	this->data = reinterpret_cast<OpenPortIpcResponse::DataBlock*>(buffer);
	this->data->header.returnCode = returnCode;
	this->data->header.errorCode = errorCode;
}

//-----------------------------------------------------------------------------
/**
	
*/
OpenPortIpcResponse::~OpenPortIpcResponse()
{
	if (this->dataOwner && this->data)
	{
		delete this->data;
		this->data = NULL;
	}
}

//-----------------------------------------------------------------------------
/**
	
*/
int
OpenPortIpcResponse::GetPortHandle()
{
	return this->data->portHandle;
}

//-----------------------------------------------------------------------------
/**

*/
struct ClosePortIpcRequest::DataBlock
{
	IpcRequestDataBlockHeader header;
	int portHandle;
};

DefinePrintJackIpcRequest(ClosePortIpcRequest)

//-----------------------------------------------------------------------------
/**
	
*/
ClosePortIpcRequest::ClosePortIpcRequest(int portHandle)
: dataOwner(true)
{
	this->data = new ClosePortIpcRequest::DataBlock();
	ZeroMemory(this->data, sizeof(ClosePortIpcRequest::DataBlock));
	this->data->header.reqType = IpcRequestType::CloseMonitorPort;
	this->data->portHandle = portHandle;
}

//-----------------------------------------------------------------------------
/**
	
*/
ClosePortIpcRequest::ClosePortIpcRequest(BYTE* buffer)
: dataOwner(false)
{
	this->data = reinterpret_cast<ClosePortIpcRequest::DataBlock*>(buffer);
}

//-----------------------------------------------------------------------------
/**
	
*/
ClosePortIpcRequest::~ClosePortIpcRequest()
{
	if (this->dataOwner && this->data)
	{
		delete this->data;
		this->data = NULL;
	}
}
	
//-----------------------------------------------------------------------------
/**
	
*/
ClosePortIpcResponse* 
ClosePortIpcRequest::GetExactResponse()
{
	if (!this->response)
		this->response = new ClosePortIpcResponse();

	return this->response;
}

//-----------------------------------------------------------------------------
/**
	
*/
int 
ClosePortIpcRequest::GetPortHandle()
{
	return this->data->portHandle;
}

//-----------------------------------------------------------------------------
/**
	
*/
struct ClosePortIpcResponse::DataBlock
{
	IpcResponseDataBlockHeader header;
};

DefinePrintJackIpcResponse(ClosePortIpcResponse)

//-----------------------------------------------------------------------------
/**
	
*/
ClosePortIpcResponse::ClosePortIpcResponse()
: dataOwner(true)
{
	this->data = new ClosePortIpcResponse::DataBlock();
	ZeroMemory(this->data, sizeof(ClosePortIpcResponse::DataBlock));
}

//-----------------------------------------------------------------------------
/**
	
*/
ClosePortIpcResponse::ClosePortIpcResponse(BYTE* buffer, DWORD returnCode, DWORD errorCode)
: dataOwner(false)
{
	this->data = reinterpret_cast<ClosePortIpcResponse::DataBlock*>(buffer);
	this->data->header.returnCode = returnCode;
	this->data->header.errorCode = errorCode;
}

//-----------------------------------------------------------------------------
/**
	
*/
ClosePortIpcResponse::~ClosePortIpcResponse()
{
	if (this->dataOwner && this->data)
	{
		delete this->data;
		this->data = NULL;
	}
}

//-----------------------------------------------------------------------------
/**

*/
struct StartDocIpcRequest::DataBlock
{
	IpcRequestDataBlockHeader header;
	int portHandle;
	wchar_t printerName[MAX_PATH];
	DWORD jobId;
	DWORD level;
};

DefinePrintJackIpcRequest(StartDocIpcRequest)

//-----------------------------------------------------------------------------
/**

*/
StartDocIpcRequest::StartDocIpcRequest(int portHandle, 
									   const wchar_t* printerName, DWORD jobId, 
									   DWORD level, const BYTE* docInfo)
: dataOwner(true)
{
	this->data = new StartDocIpcRequest::DataBlock();
	ZeroMemory(this->data, sizeof(StartDocIpcRequest::DataBlock));
	this->data->header.reqType = IpcRequestType::StartDocMonitorPort;
	this->data->portHandle = portHandle;
	wcscpy_s(this->data->printerName, sizeof(this->data->printerName), printerName);
	this->data->jobId = jobId;
	this->data->level = level;
}

//-----------------------------------------------------------------------------
/**

*/
StartDocIpcRequest::StartDocIpcRequest(BYTE* buffer)
: dataOwner(false)
{
	this->data = reinterpret_cast<StartDocIpcRequest::DataBlock*>(buffer);
}

//-----------------------------------------------------------------------------
/**

*/
StartDocIpcRequest::~StartDocIpcRequest()
{
	if (this->dataOwner && this->data)
	{
		delete this->data;
		this->data = NULL;
	}
}
	
//-----------------------------------------------------------------------------
/**

*/
StartDocIpcResponse* 
StartDocIpcRequest::GetExactResponse()
{
	if (!this->response)
		this->response = new StartDocIpcResponse();

	return this->response;
}

//-----------------------------------------------------------------------------
/**

*/
int
StartDocIpcRequest::GetPortHandle()
{
	return this->data->portHandle;
}

//-----------------------------------------------------------------------------
/**

*/
const wchar_t* 
StartDocIpcRequest::GetPrinterName()
{
	return this->data->printerName;
}

//-----------------------------------------------------------------------------
/**

*/
DWORD 
StartDocIpcRequest::GetJobId()
{
	return this->data->jobId;
}

//-----------------------------------------------------------------------------
/**
	
*/
struct StartDocIpcResponse::DataBlock
{
	IpcResponseDataBlockHeader header;
};

DefinePrintJackIpcResponse(StartDocIpcResponse)

//-----------------------------------------------------------------------------
/**
	
*/
StartDocIpcResponse::StartDocIpcResponse()
: dataOwner(true)
{
	this->data = new StartDocIpcResponse::DataBlock();
	ZeroMemory(this->data, sizeof(StartDocIpcResponse::DataBlock));
}

//-----------------------------------------------------------------------------
/**
	
*/
StartDocIpcResponse::StartDocIpcResponse(BYTE* buffer, DWORD returnCode, 
										 DWORD errorCode)
: dataOwner(false)
{
	this->data = reinterpret_cast<StartDocIpcResponse::DataBlock*>(buffer);
	this->data->header.returnCode = returnCode;
	this->data->header.errorCode = errorCode;
}

//-----------------------------------------------------------------------------
/**
	
*/
StartDocIpcResponse::~StartDocIpcResponse()
{
	if (this->dataOwner && this->data)
	{
		delete this->data;
		this->data = NULL;
	}
}

//-----------------------------------------------------------------------------
/**

*/
struct EndDocIpcRequest::DataBlock
{
	IpcRequestDataBlockHeader header;
	int portHandle;
};

DefinePrintJackIpcRequest(EndDocIpcRequest)

//-----------------------------------------------------------------------------
/**
	
*/
EndDocIpcRequest::EndDocIpcRequest(int portHandle)
: dataOwner(true)
{
	this->data = new EndDocIpcRequest::DataBlock();
	ZeroMemory(this->data, sizeof(EndDocIpcRequest::DataBlock));
	this->data->header.reqType = IpcRequestType::EndDocMonitorPort;
	this->data->portHandle = portHandle;
}

//-----------------------------------------------------------------------------
/**
	
*/
EndDocIpcRequest::EndDocIpcRequest(BYTE* buffer)
: dataOwner(false)
{
	this->data = reinterpret_cast<EndDocIpcRequest::DataBlock*>(buffer);
}

//-----------------------------------------------------------------------------
/**
	
*/
EndDocIpcRequest::~EndDocIpcRequest()
{
	if (this->dataOwner && this->data)
	{
		delete this->data;
		this->data = NULL;
	}
}
	
//-----------------------------------------------------------------------------
/**
	
*/
EndDocIpcResponse* 
EndDocIpcRequest::GetExactResponse()
{
	if (!this->response)
		this->response = new EndDocIpcResponse();

	return this->response;
}

//-----------------------------------------------------------------------------
/**
	
*/
int
EndDocIpcRequest::GetPortHandle()
{
	return this->data->portHandle;
}

//-----------------------------------------------------------------------------
/**
	
*/
struct EndDocIpcResponse::DataBlock
{
	IpcResponseDataBlockHeader header;
};

DefinePrintJackIpcResponse(EndDocIpcResponse)

//-----------------------------------------------------------------------------
/**
	
*/
EndDocIpcResponse::EndDocIpcResponse()
: dataOwner(true)
{
	this->data = new EndDocIpcResponse::DataBlock();
	ZeroMemory(this->data, sizeof(EndDocIpcResponse::DataBlock));
}

//-----------------------------------------------------------------------------
/**
	
*/
EndDocIpcResponse::EndDocIpcResponse(BYTE* buffer, DWORD returnCode, DWORD errorCode)
: dataOwner(false)
{
	this->data = reinterpret_cast<EndDocIpcResponse::DataBlock*>(buffer);
	this->data->header.returnCode = returnCode;
	this->data->header.errorCode = errorCode;
}

//-----------------------------------------------------------------------------
/**
	
*/
EndDocIpcResponse::~EndDocIpcResponse()
{
	if (this->dataOwner && this->data)
	{
		delete this->data;
		this->data = NULL;
	}
}

const DWORD WriteIpcRequest::BufferSize = 4000;

//-----------------------------------------------------------------------------
/**

*/
struct WriteIpcRequest::DataBlock
{
	IpcRequestDataBlockHeader header;
	int portHandle;
	BYTE buffer[WriteIpcRequest::BufferSize];
	DWORD numBytesInBuffer;
};

DefinePrintJackIpcRequest(WriteIpcRequest)

//-----------------------------------------------------------------------------
/**
	
*/
WriteIpcRequest::WriteIpcRequest(int portHandle)
: dataOwner(true)
{
	this->data = new WriteIpcRequest::DataBlock();
	ZeroMemory(this->data, sizeof(WriteIpcRequest::DataBlock));
	this->data->header.reqType = IpcRequestType::WriteMonitorPort;
	this->data->portHandle = portHandle;
}

//-----------------------------------------------------------------------------
/**
	
*/
WriteIpcRequest::WriteIpcRequest(BYTE* buffer)
: dataOwner(false)
{
	this->data = reinterpret_cast<WriteIpcRequest::DataBlock*>(buffer);
}

//-----------------------------------------------------------------------------
/**
	
*/
WriteIpcRequest::~WriteIpcRequest()
{
	if (this->dataOwner && this->data)
	{
		delete this->data;
		this->data = NULL;
	}
}
	
//-----------------------------------------------------------------------------
/**
	
*/
WriteIpcResponse* 
WriteIpcRequest::GetExactResponse()
{
	if (!this->response)
		this->response = new WriteIpcResponse();

	return this->response;
}

//-----------------------------------------------------------------------------
/**
	
*/
int 
WriteIpcRequest::GetPortHandle()
{
	return this->data->portHandle;
}

//-----------------------------------------------------------------------------
/**
	
*/
void 
WriteIpcRequest::SetBytes(BYTE* buffer, DWORD numBytes)
{
	memcpy_s(this->data->buffer, sizeof(this->data->buffer), buffer, numBytes);
	this->data->numBytesInBuffer = numBytes;
}

//-----------------------------------------------------------------------------
/**
	
*/
BYTE* 
WriteIpcRequest::GetBytes()
{
	return this->data->buffer;
}

//-----------------------------------------------------------------------------
/**
	
*/
DWORD 
WriteIpcRequest::GetNumBytes()
{
	return this->data->numBytesInBuffer;
}

//-----------------------------------------------------------------------------
/**
	
*/
struct WriteIpcResponse::DataBlock
{
	IpcResponseDataBlockHeader header;
	DWORD numBytesWritten;
};

DefinePrintJackIpcResponse(WriteIpcResponse)

//-----------------------------------------------------------------------------
/**
	
*/
WriteIpcResponse::WriteIpcResponse()
: dataOwner(true)
{
	this->data = new WriteIpcResponse::DataBlock();
	ZeroMemory(this->data, sizeof(WriteIpcResponse::DataBlock));
}

//-----------------------------------------------------------------------------
/**
	
*/
WriteIpcResponse::WriteIpcResponse(BYTE* buffer, DWORD returnCode, DWORD errorCode)
: dataOwner(false)
{
	this->data = reinterpret_cast<WriteIpcResponse::DataBlock*>(buffer);
	this->data->header.returnCode = returnCode;
	this->data->header.errorCode = errorCode;
}

//-----------------------------------------------------------------------------
/**
	
*/
WriteIpcResponse::~WriteIpcResponse()
{
	if (this->dataOwner && this->data)
	{
		delete this->data;
		this->data = NULL;
	}
}

//-----------------------------------------------------------------------------
/**
	
*/
DWORD 
WriteIpcResponse::GetNumBytesWritten()
{
	return this->data->numBytesWritten;
}

} // namespace printjack
