#include "precompiled.h"
#include "portproxy.h"
#include "ipcprotocol.h"

namespace printjack {

//-----------------------------------------------------------------------------
/**
	
*/
PortProxy::PortProxy(PortMonitor* monitor, const wchar_t* portName)
: monitor(monitor),
  portName(portName),
  portHandle(0)
{
	// empty
}

//-----------------------------------------------------------------------------
/**
	
*/
PortProxy::~PortProxy()
{
	this->portHandle = 0;
	this->monitor = NULL;
}

//-----------------------------------------------------------------------------
/**
	
*/
BOOL
PortProxy::Open()
{
	OpenPortIpcRequest request(this->portName.c_str());

	if (!this->pipeClient.Send(&request))
		return FALSE;
	
	OpenPortIpcResponse* response = request.GetExactResponse();

	DWORD success = response->GetReturnCode();

	if (success)
		this->portHandle = response->GetPortHandle();
	else
		SetLastError(response->GetErrorCode());

	return success;
}

//-----------------------------------------------------------------------------
/**
	
*/
BOOL 
PortProxy::StartDoc(const wchar_t* printerName, DWORD jobId, 
                    DWORD level, const BYTE* docInfo)
{
	StartDocIpcRequest request(this->portHandle, printerName, jobId, level, docInfo);

	if (!this->pipeClient.Send(&request))
		return FALSE;

	StartDocIpcResponse* response = request.GetExactResponse();

	DWORD success = response->GetReturnCode();

	if (!success)
		SetLastError(response->GetErrorCode());

	return success;
}

//-----------------------------------------------------------------------------
/**
	
*/
BOOL 
PortProxy::Write(BYTE* buffer, DWORD numBytesToWrite, DWORD* numBytesWritten)
{
	WriteIpcRequest request(this->portHandle);

	DWORD numBytesSent = 0;
	while (numBytesSent < numBytesToWrite)
	{
		DWORD numBytesToSend = min(numBytesToWrite - numBytesSent, WriteIpcRequest::BufferSize);
		request.SetBytes(buffer + numBytesSent, numBytesToSend);
		numBytesSent += numBytesToSend;

		if (!this->pipeClient.Send(&request))
			return FALSE;

		WriteIpcResponse* response = request.GetExactResponse();

		DWORD success = response->GetReturnCode();

		if (success)
			*numBytesWritten += response->GetNumBytesWritten();
		else
		{
			SetLastError(response->GetErrorCode());
			return success;
		}
	}

	return FALSE;
}

//-----------------------------------------------------------------------------
/**
	
*/
BOOL 
PortProxy::Read(BYTE* buffer, DWORD numBytesToRead, DWORD* numBytesRead)
{
	// we don't support reading!
	SetLastError(ERROR_INVALID_HANDLE);
	return FALSE;
}

//-----------------------------------------------------------------------------
/**
	
*/
BOOL
PortProxy::EndDoc()
{
	EndDocIpcRequest request(this->portHandle);

	if (!this->pipeClient.Send(&request))
		return FALSE;

	EndDocIpcResponse* response = request.GetExactResponse();

	DWORD success = response->GetReturnCode();

	if (!success)
		SetLastError(response->GetErrorCode());

	return success;
}

//-----------------------------------------------------------------------------
/**
	
*/
BOOL
PortProxy::Close()
{
	ClosePortIpcRequest request(this->portHandle);

	if (!this->pipeClient.Send(&request))
		return FALSE;
	
	ClosePortIpcResponse* response = request.GetExactResponse();

	DWORD success = response->GetReturnCode();

	if (!success)
		SetLastError(response->GetErrorCode());

	return success;
}

} // namespace printjack
