#include "precompiled.h"
#include "PipeServer.h"
#include "PipeConnection.h"
#include "IpcHandler.h"

namespace PrintJackServiceMixed {

namespace {

//-----------------------------------------------------------------------------
/**
	
*/
void
WINAPI
CompletedRead(DWORD errorCode, DWORD bytesRead, OVERLAPPED* overlapped)
{
	PipeConnection* con = reinterpret_cast<PipeConnection*>(overlapped);
	con->server->Get()->OnReadCompleted(errorCode, bytesRead, con);
}

//-----------------------------------------------------------------------------
/**
	
*/
void
WINAPI
CompletedWrite(DWORD errorCode, DWORD bytesWritten, OVERLAPPED* overlapped)
{
	PipeConnection* con = reinterpret_cast<PipeConnection*>(overlapped);
	con->server->Get()->OnWriteCompleted(errorCode, bytesWritten, con);
}

} // anonymous namespace

//-----------------------------------------------------------------------------
/**

*/
PipeServer::PipeServer(IpcHandler^ ipcHandler)
: terminationEventHandle(NULL),
  pendingConnectionHandle(NULL),
  connectionPending(false),
  ipcHandler(ipcHandler)
{
	this->overlappedConnect = new OVERLAPPED();
	ZeroMemory(this->overlappedConnect, sizeof(this->overlappedConnect));
}

//-----------------------------------------------------------------------------
/**
	
*/
PipeServer::~PipeServer()
{
	// just in case Start() was called but Step() and Stop() weren't
	this->DoStop();
	
	if (this->overlappedConnect)
	{
		delete this->overlappedConnect;
		this->overlappedConnect = NULL;
	}
}

//-----------------------------------------------------------------------------
/**
	
*/
void
PipeServer::Start()
{
	// create the event used to wait for a termination request
	this->terminationEventHandle = CreateEvent(
		NULL,  // security attributes
		TRUE,  // manual-reset
		FALSE, // initially non-signaled
		NULL   // unnamed
	);
	
	if (!this->terminationEventHandle)
	{
		// fixme: throw GetLastError()
		return;
	}

	// create the event used to wait for a client connection
	this->overlappedConnect->hEvent = CreateEvent(
		NULL, // security attributes
		TRUE, // manual-reset
		TRUE, // initially signaled
		NULL  // unnamed
	);
	
	if (!this->overlappedConnect->hEvent)
	{
		// fixme: throw GetLastError()
		return;
	}
	
	this->OpenPipe();
}

//-----------------------------------------------------------------------------
/**
	@return true if this method should be called again because the server is
	        still running, false if this method should not be called again
	        because the server has stopped.
*/
bool
PipeServer::Step()
{
	HANDLE events[] = { 
		this->terminationEventHandle, 
		this->overlappedConnect->hEvent 
	};

	// wait for either:
	// - a client to connect
	// - a read or write operation to be completed
	// - a request for the server to terminate
	DWORD waitStatus = WaitForMultipleObjectsEx(
		2, // number of objects in the array 
		events, // objects to wait for 
		FALSE, // don't wait for all object to be signaled, one will do
		INFINITE, // waits indefinitely 
		TRUE // return when a completion routine is queued by the OS
	);

	switch (waitStatus) 
	{
		// the termination event was signaled
		case WAIT_OBJECT_0:
			this->DoStop();
			return false;
			
		// the connection event was signaled
		case (WAIT_OBJECT_0 + 1):
			this->EstablishConnection();
			// create new pipe for the next client
			this->OpenPipe();
			break; 

		// a read or write operation has completed
		case WAIT_IO_COMPLETION: 
			break;

		// an error occurred
		default: 
			// fixme: throw GetLastError()
			break;
	}
	
	return true;
}

//-----------------------------------------------------------------------------
/**
	@brief Request the server to close all connections and stop listening.
	
	This method can be called from any thread, including the one running the 
	server. It is non-blocking and will return immediately, however,
	the server may take some time to actually stop.
*/
void
PipeServer::Stop()
{
	SetEvent(this->terminationEventHandle);
}

//-----------------------------------------------------------------------------
/**
	@brief Raise the ReadCompleted event.
*/
void
PipeServer::OnReadCompleted(DWORD errorCode, DWORD bytesRead, 
                            PipeConnection* connection)
{
	BOOL success = FALSE;
	
	// a read operation has finished, write a response
	if ((0 == errorCode) && (bytesRead != 0)) 
	{ 
		connection->numOutputBufferBytes = this->ipcHandler->ProcessRequest(
			connection->inputBuffer, connection->outputBuffer
		);
		// raise the ReadCompleted event
		//this->ReadCompleted(this);

		success = WriteFileEx( 
			connection->pipeHandle, 
			connection->outputBuffer, 
			connection->numOutputBufferBytes, 
			reinterpret_cast<OVERLAPPED*>(connection),
			(LPOVERLAPPED_COMPLETION_ROUTINE)CompletedWrite
		); 
	} 

	if (!success) 
		this->ClosePipe(connection);
}

//-----------------------------------------------------------------------------
/**
	@brief Raise the WriteCompleted event.
*/
void
PipeServer::OnWriteCompleted(DWORD errorCode, DWORD bytesWritten, 
							 PipeConnection* connection)
{
	BOOL success = FALSE;
	
	// a write operation has finished, read the next request
	if ((0 == errorCode) && (bytesWritten == connection->numOutputBufferBytes))
	{
		// raise the WriteCompleted event
		//this->WriteCompleted(this);

		success = ReadFileEx(
			connection->pipeHandle,
			connection->inputBuffer, 
			sizeof(connection->inputBuffer),
			reinterpret_cast<OVERLAPPED*>(connection), 
			(LPOVERLAPPED_COMPLETION_ROUTINE)CompletedRead
		); 
	}

	if (!success) 
		this->ClosePipe(connection);
}

//-----------------------------------------------------------------------------
/**
	
*/
void
PipeServer::OpenPipe()
{
	this->connectionPending = false;

	const wchar_t* pipeName = L"\\\\.\\pipe\\printjack"; 

	this->pendingConnectionHandle = CreateNamedPipe( 
		pipeName,                 // pipe name 
		PIPE_ACCESS_DUPLEX |      // read/write access 
		FILE_FLAG_OVERLAPPED,     // overlapped mode 
		PIPE_TYPE_MESSAGE |       // message-type pipe 
		PIPE_READMODE_MESSAGE |   // message read mode 
		PIPE_WAIT,                // blocking mode 
		PIPE_UNLIMITED_INSTANCES, // unlimited instances 
		PIPE_BUFFER_SIZE,         // output buffer size 
		PIPE_BUFFER_SIZE,         // input buffer size 
		1000,                     // client time-out 
		NULL                      // default security attributes
	);                    
	
	if (INVALID_HANDLE_VALUE == this->pendingConnectionHandle)
	{
		// fixme: throw GetLastError()
		return;
	}

	// start an overlapped connection...
	// While the MSDN docs fail to mention exactly what happens to
	// OVERLAPPED::hEvent the example code leads me to make the 
	// following assumptions:
	// - The event is set to non-signaled by ConnectNamedPipe().
	// - If GetLastError() returns ERROR_IO_PENDING then the event will be set 
	//   to signaled by the OS when a connection actually occurs.
	// - If GetLastError() return ERROR_PIPE_CONNECTED then a connection 
	//   already exists and it's our responsibility to set the event to 
	//   signaled.
	int retval = ConnectNamedPipe(this->pendingConnectionHandle, this->overlappedConnect);

	// overlapped ConnectNamedPipe should return zero
	if (0 == retval)
	{
		// fixme: throw GetLastError()
		return;
	}

	switch (GetLastError()) 
	{ 
		// still waiting for a connection...
		case ERROR_IO_PENDING:
			this->connectionPending = true; 
			break; 

		// client has already connected between the calls to
		// CreateNamedPipe() and ConnectNamedPipe()
		case ERROR_PIPE_CONNECTED: 
			if (SetEvent(this->overlappedConnect->hEvent)) 
				break;

		default:
			// fixme: throw GetLastError()
			return;
	}
}

//-----------------------------------------------------------------------------
/**
	
*/
void
PipeServer::EstablishConnection()
{
	if (this->connectionPending) 
	{
		DWORD unused;
		BOOL success = GetOverlappedResult( 
			this->pendingConnectionHandle,
			this->overlappedConnect,
			&unused,
			FALSE // don't wait
		);
		if (!success)
		{
			// fixme: throw GetLastError()
			return;
		}
	} 

	// raise the ClientConnected event
	//this->ClientConnected(this);

	// fixme: replace with a smart pointer
	PipeConnection* connection = new PipeConnection();
	connection->server = new PipeServerWrapper(this);
	connection->pipeHandle = this->pendingConnectionHandle;

	// start the read operation for this client
	BOOL success = ReadFileEx( 
		connection->pipeHandle,
		connection->inputBuffer, 
		sizeof(connection->inputBuffer), 
		reinterpret_cast<OVERLAPPED*>(connection),
		(LPOVERLAPPED_COMPLETION_ROUTINE)CompletedRead
	);

	if (!success)
		this->ClosePipe(connection);
}

//-----------------------------------------------------------------------------
/**
	@brief Cleanup any resources created in OpenPipe() & EstablishConnection().
*/
void
PipeServer::ClosePipe(PipeConnection* connection)
{
	if (!DisconnectNamedPipe(connection->pipeHandle))
	{
		// fixme: throw GetLastError()
	}

	CloseHandle(connection->pipeHandle);

	delete connection->server;
	delete connection;
}

//-----------------------------------------------------------------------------
/**
	@brief Cleanup any resources created in Start().
*/
void
PipeServer::DoStop()
{
	if (this->overlappedConnect->hEvent)
	{
		CloseHandle(this->overlappedConnect->hEvent);
		this->overlappedConnect->hEvent = NULL;
	}

	if (this->terminationEventHandle)
	{
		CloseHandle(this->terminationEventHandle);
		this->terminationEventHandle = NULL;
	}
}

} // namespace PrintJackServiceMixed
