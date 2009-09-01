#pragma once

namespace printjack {

class IpcRequest;
class IpcResponse;

class PipeClient
{
public:
	PipeClient();
	~PipeClient();

	bool Connect();
	bool Send(IpcRequest* request);
	void Disconnect();
	
private:
	HANDLE pipeHandle;
};

} // namespace printjack
