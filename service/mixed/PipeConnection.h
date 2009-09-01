#pragma once

namespace PrintJackServiceMixed {

//-----------------------------------------------------------------------------
/**
	@class PipeServerWrapper
	@brief Native class that wraps a pointer to a managed PipeServer instance.
*/
class PipeServerWrapper
{
public:
	PipeServerWrapper(PipeServer^ ptr)
	: ptr(ptr)
	{
		// empty
	}
	
	PipeServer^ Get()
	{
		return static_cast<PipeServer^>(ptr);
	}
	
private:
	msclr::gcroot<PipeServer^> ptr;
};

const int PIPE_BUFFER_SIZE = 4096;

//-----------------------------------------------------------------------------
/**
	@struct PipeConnection
	@brief Represents a pipe connection.
*/
struct PipeConnection
{
	OVERLAPPED overlapped; // used by functions that support overlapped IO
	PipeServerWrapper* server;
	HANDLE pipeHandle; // server end of the pipe instance
	BYTE inputBuffer[PIPE_BUFFER_SIZE];
	BYTE outputBuffer[PIPE_BUFFER_SIZE];
	DWORD numOutputBufferBytes;
};

} // namespace PrintJackServiceMixed
