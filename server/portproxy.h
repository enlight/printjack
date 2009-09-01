#pragma once

#include "pipeclient.h"

namespace printjack {

class PortMonitor;

class PortProxy
{
public:
	PortProxy(PortMonitor* monitor, const wchar_t* portName);
	~PortProxy();
	
	BOOL Open();
	BOOL StartDoc(const wchar_t* printerName, DWORD jobId, DWORD level, const BYTE* docInfo);
	BOOL Write(BYTE* buffer, DWORD numBytesToWrite, DWORD* numBytesWritten);
	BOOL Read(BYTE* buffer, DWORD numBytesToRead, DWORD* numBytesRead);
	BOOL EndDoc();
	BOOL Close();
	
private:
	PortMonitor* monitor;
	std::wstring portName;
	PipeClient pipeClient;
	int portHandle;
};

} // namespace printjack
