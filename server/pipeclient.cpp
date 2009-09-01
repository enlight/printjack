#include "precompiled.h"
#include "pipeclient.h"
#include "ipcprotocol.h"

namespace printjack {

//-----------------------------------------------------------------------------
/**
	
*/
PipeClient::PipeClient()
: pipeHandle(NULL)
{
	// empty
}

PipeClient::~PipeClient()
{
	if (this->pipeHandle)
		this->Disconnect();
}

//-----------------------------------------------------------------------------
/**
	
*/
bool
PipeClient::Connect()
{
	const wchar_t* pipeName = L"\\\\.\\pipe\\printjack";
	
	while (true) 
	{ 
		HANDLE hPipe = CreateFile( 
			pipeName,       // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE, 
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // open existing pipe 
			0,              // default attributes 
			NULL            // no template file 
		);

		// break if the pipe handle is valid
		if (hPipe != INVALID_HANDLE_VALUE)
		{
			this->pipeHandle = hPipe;
			break;
		}

		if (ERROR_PIPE_BUSY == GetLastError())
		{
			// all pipe instances are busy, wait for a second. 
			if (!WaitNamedPipe(pipeName, 1000)) 
				return false;	
		}
		else // some other error
			return false;
	}
	
	// the pipe connected, change to message-read mode
	DWORD dwMode = PIPE_READMODE_MESSAGE;
	BOOL success = SetNamedPipeHandleState( 
		this->pipeHandle,
		&dwMode,
		NULL, // must be null because client & server are on the same machine
		NULL  // must be null because client & server are on the same machine
	);
	
	if (success)
		return true;
	else
		return false;
}

//-----------------------------------------------------------------------------
/**
	
*/
bool
PipeClient::Send(IpcRequest* request)
{
	// open a connection if we haven't done so earlier
	if (!this->pipeHandle)
		this->Connect();

	IpcResponse* response = request->GetResponse();
	DWORD bytesRead = 0;
	
	BOOL success = TransactNamedPipe(
		this->pipeHandle,
		request->GetData(), request->GetDataSize(),
		response->GetData(), response->GetDataSize(),
		&bytesRead, NULL
	);
	
	if (success)
	{
		return true;
	}
	else if (ERROR_MORE_DATA == GetLastError())
	{
		if (!response->AllocateExtraData())
			return false;
		
		success = ReadFile(
			this->pipeHandle,
			response->GetExtraData(),
			response->GetExtraDataSize(),
			&bytesRead,	NULL
		);

		// in theory we should keep reading if success is FALSE and
		// GetLastError() returns ERROR_MORE_DATA... but for our needs 
		// this isn't necessary
		if (success)
			return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
/**
	
*/
void 
PipeClient::Disconnect()
{
	if (this->pipeHandle)
	{
		CloseHandle(this->pipeHandle);
		this->pipeHandle = NULL;
	}
}

} // namespace printjack
