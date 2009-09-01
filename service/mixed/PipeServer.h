#pragma once

using namespace System;
using namespace System::Threading;

namespace PrintJackServiceMixed {

struct PipeConnection;
ref class IpcHandler;

public ref class PipeServer
{
public:
/*
	delegate void ClientConnectedEventHandler(Object^ sender);
	delegate void ReadCompletedEventHandler(Object^ sender);
	delegate void WriteCompletedEventHandler(Object^ sender);
	
	event ClientConnectedEventHandler^ ClientConnected;
	event ReadCompletedEventHandler^ ReadCompleted;
	event WriteCompletedEventHandler^ WriteCompleted;
*/	
	PipeServer(IpcHandler^ ipcHandler);
	~PipeServer();
	
	void Start();
	bool Step();
	void Stop();
	
internal:
	void OnReadCompleted(DWORD errorCode, DWORD bytesRead, PipeConnection* connection);
	void OnWriteCompleted(DWORD errorCode, DWORD bytesWritten, PipeConnection* connection);
	
private:
	void OpenPipe();
	void EstablishConnection();
	void ClosePipe(PipeConnection* connection);
	void DoStop();

	HANDLE terminationEventHandle;
	OVERLAPPED* overlappedConnect;
	HANDLE pendingConnectionHandle;
	bool connectionPending;
	IpcHandler^ ipcHandler;
};

} // namespace PrintJackServiceMixed
