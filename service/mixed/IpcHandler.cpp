#include "precompiled.h"
#include "IpcHandler.h"
#include "..\..\server\ipcprotocol.h"

using namespace printjack;

namespace PrintJackServiceMixed {

DWORD
IpcHandler::ProcessRequest(BYTE* inputBuffer, BYTE* outputBuffer)
{
	__int32 reqType = *(reinterpret_cast<__int32*>(inputBuffer));
	switch (reqType)
	{
		case IpcRequestType::AddMonitorPort:
			return this->OnAddMonitorPort(inputBuffer, outputBuffer);
				
		case IpcRequestType::DeleteMonitorPort:
			return this->OnDeleteMonitorPort(inputBuffer, outputBuffer);

		case IpcRequestType::OpenMonitorPort:
			return this->OnOpenMonitorPort(inputBuffer, outputBuffer);

		case IpcRequestType::CloseMonitorPort:
			return this->OnCloseMonitorPort(inputBuffer, outputBuffer);

		case IpcRequestType::StartDocMonitorPort:
			return this->OnStartDocMonitorPort(inputBuffer, outputBuffer);

		case IpcRequestType::EndDocMonitorPort:
			return this->OnEndDocMonitorPort(inputBuffer, outputBuffer);

		case IpcRequestType::WriteMonitorPort:
			return this->OnWriteMonitorPort(inputBuffer, outputBuffer);
	}
	return 0; // fixme
}

DWORD 
IpcHandler::OnAddMonitorPort(BYTE* inputBuffer, BYTE* outputBuffer)
{
	AddPortIpcRequest request(inputBuffer);
	AddPortEventArgs^ e = gcnew AddPortEventArgs();
	e->portName = gcnew String(request.GetPortName());
	this->AddMonitorPort(this, e);
	AddPortIpcResponse response(outputBuffer, e->returnCode, e->errorCode);
	return response.GetDataSize();
}

DWORD 
IpcHandler::OnDeleteMonitorPort(BYTE* inputBuffer, BYTE* outputBuffer)
{
	DeletePortIpcRequest request(inputBuffer);
	DeletePortEventArgs^ e = gcnew DeletePortEventArgs();
	e->portName = gcnew String(request.GetPortName());
	this->DeleteMonitorPort(this, e);
	DeletePortIpcResponse response(outputBuffer, e->returnCode, e->errorCode);
	return response.GetDataSize();
}

DWORD
IpcHandler::OnOpenMonitorPort(BYTE* inputBuffer, BYTE* outputBuffer)
{
	OpenPortIpcRequest request(inputBuffer);
	OpenPortEventArgs^ e = gcnew OpenPortEventArgs();
	e->portName = gcnew String(request.GetPortName());
	this->OpenMonitorPort(this, e);
	OpenPortIpcResponse response(outputBuffer, e->returnCode, e->errorCode);
	return response.GetDataSize();
}

DWORD
IpcHandler::OnCloseMonitorPort(BYTE* inputBuffer, BYTE* outputBuffer)
{
	ClosePortIpcRequest request(inputBuffer);
	ClosePortEventArgs^ e = gcnew ClosePortEventArgs();
	e->portHandle = request.GetPortHandle();
	this->CloseMonitorPort(this, e);
	ClosePortIpcResponse response(outputBuffer, e->returnCode, e->errorCode);
	return response.GetDataSize();
}

DWORD 
IpcHandler::OnStartDocMonitorPort(BYTE* inputBuffer, BYTE* outputBuffer)
{
	StartDocIpcRequest request(inputBuffer);
	StartDocEventArgs^ e = gcnew StartDocEventArgs();
	e->portHandle = request.GetPortHandle();
	e->printerName = gcnew String(request.GetPrinterName());
	e->jobId = request.GetJobId();
	this->StartDocMonitorPort(this, e);
	StartDocIpcResponse response(outputBuffer, e->returnCode, e->errorCode);
	return response.GetDataSize();
}

DWORD 
IpcHandler::OnEndDocMonitorPort(BYTE* inputBuffer, BYTE* outputBuffer)
{
	EndDocIpcRequest request(inputBuffer);
	EndDocEventArgs^ e = gcnew EndDocEventArgs();
	e->portHandle = request.GetPortHandle();
	this->EndDocMonitorPort(this, e);
	EndDocIpcResponse response(outputBuffer, e->returnCode, e->errorCode);
	return response.GetDataSize();
}

DWORD
IpcHandler::OnWriteMonitorPort(BYTE* inputBuffer, BYTE* outputBuffer)
{
	WriteIpcRequest request(inputBuffer);
	WriteEventArgs^ e = gcnew WriteEventArgs();
	e->portHandle = request.GetPortHandle();
	e->buffer = gcnew IntPtr(request.GetBytes());
	e->bufferSize = request.GetNumBytes();
	this->WriteMonitorPort(this, e);
	WriteIpcResponse response(outputBuffer, e->returnCode, e->errorCode);
	return response.GetDataSize();
}

} // namespace PrintJackServiceMixed
