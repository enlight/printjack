#pragma once

namespace printjack {

#define DeclarePrintJackIpcRequest \
public:                            \
	IpcResponse* GetResponse();    \
	void* GetData();               \
	DWORD GetDataSize();              
	
#define DefinePrintJackIpcRequest(className)                                   \
	IpcResponse* className::GetResponse() { return this->GetExactResponse(); } \
	void* className::GetData() { return this->data; }                          \
	DWORD className::GetDataSize() { return sizeof(className::DataBlock); }

#define DeclarePrintJackIpcResponse \
public:                             \
	void* GetData();                \
	DWORD GetDataSize();                                              

#define DefinePrintJackIpcResponse(className)                              \
	void* className::GetData() { return this->data; }                      \
	DWORD className::GetDataSize() { return sizeof(className::DataBlock); }

//#define DefinePrintJackIpcGetData(className, dataBlockClassName)           \
//	void* className::GetData() { return this->data; }                      \
//	DWORD className::GetDataSize() { return sizeof(dataBlockClassName); }

//-----------------------------------------------------------------------------
/**
	
*/
namespace IpcRequestType
{
	enum Type
	{
		Invalid = 0,
		AddMonitorPort,
		DeleteMonitorPort,
		OpenMonitorPort,
		CloseMonitorPort,
		StartDocMonitorPort,
		EndDocMonitorPort,
		WriteMonitorPort,
	};
} // namespace IpcRequestType

//-----------------------------------------------------------------------------
/**
	
*/
struct IpcRequestDataBlockHeader
{
	__int32 reqType;
};

class IpcResponse;

class IpcRequest
{
public:
	virtual ~IpcRequest() = 0;
	/// get the response to the request, this is only valid after the request
	/// is successfully sent, the response will be automatically destroyed with
	/// the request
	virtual IpcResponse* GetResponse() = 0;
	/// get the location from which the pipe client should read data from
	virtual void* GetData() = 0;
	/// get the number of bytes that should be read from location returned
	/// by GetData()
	virtual DWORD GetDataSize() = 0;
};

//-----------------------------------------------------------------------------
/**

*/
struct IpcResponseDataBlockHeader
{
	DWORD returnCode;
	DWORD errorCode;
};

//-----------------------------------------------------------------------------
/**

*/
class IpcResponse
{
public:
	virtual ~IpcResponse() = 0;
	/// get the location where the pipe client should write data to
	virtual void* GetData() = 0;
	/// get the maximum number of bytes that can be written to the location
	/// returned by GetData()
	virtual DWORD GetDataSize() = 0;
	/// allocate memory that the pipe client can store additional data in,
	/// should be called prior to GetExtraData() and GetExtraDataSize()
	virtual bool AllocateExtraData() { return false; }
	/// get the location where the pipe client should write extra data to
	virtual void* GetExtraData() { return NULL; }
	/// get the maximum number of bytes that can be written to the location
	/// returned by GetExtraData()
	virtual DWORD GetExtraDataSize() { return 0; }
	/// get the return code of the operation performed by the request
	DWORD GetReturnCode();
	/// get the error code, this is only valid if the return code is zero
	DWORD GetErrorCode();
};

//-----------------------------------------------------------------------------
/**

*/
inline
DWORD
IpcResponse::GetReturnCode()
{
	IpcResponseDataBlockHeader* header = static_cast<IpcResponseDataBlockHeader*>(this->GetData());
	if (header)
		return header->returnCode;
	else
		return 0;
}

//-----------------------------------------------------------------------------
/**

*/
inline
DWORD
IpcResponse::GetErrorCode()
{
	IpcResponseDataBlockHeader* header = static_cast<IpcResponseDataBlockHeader*>(this->GetData());
	if (header)
		return header->errorCode;
	else
		return 0;
}

//-----------------------------------------------------------------------------
/**

*/
class AddPortIpcResponse : public IpcResponse
{
	DeclarePrintJackIpcResponse

public:
	AddPortIpcResponse();
	AddPortIpcResponse(BYTE* buffer, DWORD returnCode, DWORD errorCode);
	~AddPortIpcResponse();

private:
	struct DataBlock;
	DataBlock* data;
	bool dataOwner;
};

//-----------------------------------------------------------------------------
/**
	
*/
class AddPortIpcRequest : public IpcRequest
{
	DeclarePrintJackIpcRequest

public:
	AddPortIpcRequest(const wchar_t* portName);
	AddPortIpcRequest(BYTE* buffer);
	~AddPortIpcRequest();

	AddPortIpcResponse* GetExactResponse();
	const wchar_t* GetPortName();
	
private:
	struct DataBlock;
	DataBlock* data;
	bool dataOwner;
	AddPortIpcResponse* response;
};

//-----------------------------------------------------------------------------
/**

*/
class DeletePortIpcResponse : public IpcResponse
{
	DeclarePrintJackIpcResponse

public:
	DeletePortIpcResponse();
	DeletePortIpcResponse(BYTE* buffer, DWORD returnCode, DWORD errorCode);
	~DeletePortIpcResponse();

private:
	struct DataBlock;
	DataBlock* data;
	bool dataOwner;
};

//-----------------------------------------------------------------------------
/**

*/
class DeletePortIpcRequest : public IpcRequest
{
	DeclarePrintJackIpcRequest

public:
	DeletePortIpcRequest(const wchar_t* portName);
	DeletePortIpcRequest(BYTE* buffer);
	~DeletePortIpcRequest();

	DeletePortIpcResponse* GetExactResponse();
	const wchar_t* GetPortName();

private:
	struct DataBlock;
	DataBlock* data;
	bool dataOwner;
	DeletePortIpcResponse* response;
};

//-----------------------------------------------------------------------------
/**

*/
/*
class EnumPortsIpcResponse : public IpcResponse
{
public:
	EnumPortsIpcResponse(void* portsData, DWORD portsDataSize);
	~EnumPortsIpcResponse();

	DeclarePrintJackIpcGetData()
	
	bool AllocateExtraData();
	void* GetExtraData();
	DWORD GetExtraDataSize();

	DWORD GetNumBytesNeeded();
	DWORD GetNumPorts();

private:
	struct DataBlock;
	DataBlock* data;
	
	void* portsData;
	DWORD portsDataSize;
};
*/
//-----------------------------------------------------------------------------
/**
	
*/
/*
class EnumPortsIpcRequest : public IpcRequest
{
public:
	EnumPortsIpcRequest(DWORD level, BYTE* portsBuffer, DWORD portsBufferSize);
	~EnumPortsIpcRequest();
	
	IpcResponse* GetResponse();
	
	DeclarePrintJackIpcGetData()
	
private:
	struct DataBlock;
	DataBlock* data;
	
	BYTE* portsBuffer;
	DWORD portsBufferSize;
};
*/
//-----------------------------------------------------------------------------
/**
	
*/
class OpenPortIpcResponse : public IpcResponse
{
	DeclarePrintJackIpcResponse

public:
	OpenPortIpcResponse();
	OpenPortIpcResponse(BYTE* buffer, DWORD returnCode, DWORD errorCode);
	~OpenPortIpcResponse();
	
	int GetPortHandle();
	
private:
	struct DataBlock;
	DataBlock* data;
	bool dataOwner;
};

//-----------------------------------------------------------------------------
/**
	
*/
class OpenPortIpcRequest : public IpcRequest
{
	DeclarePrintJackIpcRequest

public:
	OpenPortIpcRequest(const wchar_t* portName);
	OpenPortIpcRequest(BYTE* buffer);
	~OpenPortIpcRequest();
	
	OpenPortIpcResponse* GetExactResponse();
	const wchar_t* GetPortName();

private:
	struct DataBlock;
	DataBlock* data;
	bool dataOwner;
	OpenPortIpcResponse* response;
};

//-----------------------------------------------------------------------------
/**
	
*/
class ClosePortIpcResponse : public IpcResponse
{
	DeclarePrintJackIpcResponse

public:
	ClosePortIpcResponse();
	ClosePortIpcResponse(BYTE* buffer, DWORD returnCode, DWORD errorCode);
	~ClosePortIpcResponse();
	
private:
	struct DataBlock;
	DataBlock* data;
	bool dataOwner;
};

//-----------------------------------------------------------------------------
/**
	
*/
class ClosePortIpcRequest : public IpcRequest
{
	DeclarePrintJackIpcRequest

public:
	ClosePortIpcRequest(int portHandle);
	ClosePortIpcRequest(BYTE* buffer);
	~ClosePortIpcRequest();
	
	ClosePortIpcResponse* GetExactResponse();
	int GetPortHandle();

private:
	struct DataBlock;
	DataBlock* data;
	bool dataOwner;
	ClosePortIpcResponse* response;
};

//-----------------------------------------------------------------------------
/**
	
*/
class StartDocIpcResponse : public IpcResponse
{
	DeclarePrintJackIpcResponse

public:
	StartDocIpcResponse();
	StartDocIpcResponse(BYTE* buffer, DWORD returnCode, DWORD errorCode);
	~StartDocIpcResponse();
	
private:
	struct DataBlock;
	DataBlock* data;
	bool dataOwner;
};

//-----------------------------------------------------------------------------
/**
	
*/
class StartDocIpcRequest : public IpcRequest
{
	DeclarePrintJackIpcRequest

public:
	StartDocIpcRequest(int portHandle, const wchar_t* printerName, DWORD jobId, DWORD level, const BYTE* docInfo);
	StartDocIpcRequest(BYTE* buffer);
	~StartDocIpcRequest();
	
	StartDocIpcResponse* GetExactResponse();
	int GetPortHandle();
	const wchar_t* GetPrinterName();
	DWORD GetJobId();

private:
	struct DataBlock;
	DataBlock* data;
	bool dataOwner;
	StartDocIpcResponse* response;
};

//-----------------------------------------------------------------------------
/**
	
*/
class EndDocIpcResponse : public IpcResponse
{
	DeclarePrintJackIpcResponse

public:
	EndDocIpcResponse();
	EndDocIpcResponse(BYTE* buffer, DWORD returnCode, DWORD errorCode);
	~EndDocIpcResponse();
	
private:
	struct DataBlock;
	DataBlock* data;
	bool dataOwner;
};

//-----------------------------------------------------------------------------
/**
	
*/
class EndDocIpcRequest : public IpcRequest
{
	DeclarePrintJackIpcRequest

public:
	EndDocIpcRequest(int portHandle);
	EndDocIpcRequest(BYTE* buffer);
	~EndDocIpcRequest();
	
	EndDocIpcResponse* GetExactResponse();
	int GetPortHandle();

private:
	struct DataBlock;
	DataBlock* data;
	bool dataOwner;
	EndDocIpcResponse* response;
};

//-----------------------------------------------------------------------------
/**
	
*/
class WriteIpcResponse : public IpcResponse
{
	DeclarePrintJackIpcResponse

public:
	WriteIpcResponse();
	WriteIpcResponse(BYTE* buffer, DWORD returnCode, DWORD errorCode);
	~WriteIpcResponse();

	DWORD GetNumBytesWritten();
	
private:
	struct DataBlock;
	DataBlock* data;
	bool dataOwner;
};

//-----------------------------------------------------------------------------
/**
	
*/
class WriteIpcRequest : public IpcRequest
{
	DeclarePrintJackIpcRequest

public:
	WriteIpcRequest(int portHandle);
	WriteIpcRequest(BYTE* buffer);
	~WriteIpcRequest();
	
	WriteIpcResponse* GetExactResponse();
	int GetPortHandle();
	void SetBytes(BYTE* buffer, DWORD numBytes);
	BYTE* GetBytes();
	DWORD GetNumBytes();

	static const DWORD BufferSize;

private:
	struct DataBlock;
	DataBlock* data;
	bool dataOwner;
	WriteIpcResponse* response;
};

} // namespace printjack
