using namespace System;

namespace PrintJackServiceMixed {

public ref class IpcHandlerEventArgs : public System::EventArgs
{
public:
	int returnCode;
	int errorCode;
};

public ref class AddPortEventArgs : public IpcHandlerEventArgs
{
public:
	String^ portName;
};

public ref class DeletePortEventArgs : public IpcHandlerEventArgs
{
public:
	String^ portName;
};

public ref class OpenPortEventArgs : public IpcHandlerEventArgs
{
public:
	String^ portName;
};

public ref class ClosePortEventArgs : public IpcHandlerEventArgs
{
public:
	int portHandle;
};

public ref class StartDocEventArgs : public IpcHandlerEventArgs
{
public:
	int portHandle;
	String^ printerName;
	long jobId;
};

public ref class EndDocEventArgs : public IpcHandlerEventArgs
{
public:
	int portHandle;
};

public ref class WriteEventArgs : public IpcHandlerEventArgs
{
public:
	int portHandle;
	IntPtr^ buffer;
	int bufferSize;
};

public ref class IpcHandler
{
public:
	delegate void AddMonitorPortEventHandler(Object^ sender, AddPortEventArgs^ e);
	delegate void DeleteMonitorPortEventHandler(Object^ sender, DeletePortEventArgs^ e);
	delegate void OpenMonitorPortEventHandler(Object^ sender, OpenPortEventArgs^ e);
	delegate void CloseMonitorPortEventHandler(Object^ sender, ClosePortEventArgs^ e);
	delegate void StartDocMonitorPortEventHandler(Object^ sender, StartDocEventArgs^ e);
	delegate void EndDocMonitorPortEventHandler(Object^ sender, EndDocEventArgs^ e);
	delegate void WriteMonitorPortEventHandler(Object^ sender, WriteEventArgs^ e);

	event AddMonitorPortEventHandler^ AddMonitorPort;
	event DeleteMonitorPortEventHandler^ DeleteMonitorPort;
	event OpenMonitorPortEventHandler^ OpenMonitorPort;
	event CloseMonitorPortEventHandler^ CloseMonitorPort;
	event StartDocMonitorPortEventHandler^ StartDocMonitorPort;
	event EndDocMonitorPortEventHandler^ EndDocMonitorPort;
	event WriteMonitorPortEventHandler^ WriteMonitorPort;

internal:
	DWORD ProcessRequest(BYTE* inputBuffer, BYTE* outputBuffer);
	
private:
	DWORD OnAddMonitorPort(BYTE* inputBuffer, BYTE* outputBuffer);
	DWORD OnDeleteMonitorPort(BYTE* inputBuffer, BYTE* outputBuffer);
	DWORD OnOpenMonitorPort(BYTE* inputBuffer, BYTE* outputBuffer);
	DWORD OnCloseMonitorPort(BYTE* inputBuffer, BYTE* outputBuffer);
	DWORD OnStartDocMonitorPort(BYTE* inputBuffer, BYTE* outputBuffer);
	DWORD OnEndDocMonitorPort(BYTE* inputBuffer, BYTE* outputBuffer);
	DWORD OnWriteMonitorPort(BYTE* inputBuffer, BYTE* outputBuffer);
};

} // namespace PrintJackServiceMixed
