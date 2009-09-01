using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using PrintJackServiceMixed;

namespace PrintJackService {

class ServerThreadState
{
	private PipeServer pipeServer;
	private PortMonitor portMonitor;

	public ServerThreadState()
	{
		// empty
	}
	
	public void Start()
	{
		this.portMonitor = new PortMonitor();
	
		IpcHandler ipcHandler = new IpcHandler();
		ipcHandler.AddMonitorPort += new IpcHandler.AddMonitorPortEventHandler(portMonitor.AddPort);
		ipcHandler.DeleteMonitorPort += new IpcHandler.DeleteMonitorPortEventHandler(portMonitor.DeletePort);
        ipcHandler.OpenMonitorPort += new IpcHandler.OpenMonitorPortEventHandler(portMonitor.OpenPort);
        ipcHandler.CloseMonitorPort += new IpcHandler.CloseMonitorPortEventHandler(portMonitor.ClosePort);
        ipcHandler.StartDocMonitorPort += new IpcHandler.StartDocMonitorPortEventHandler(portMonitor.StartDoc);
        ipcHandler.EndDocMonitorPort += new IpcHandler.EndDocMonitorPortEventHandler(portMonitor.EndDoc);
        ipcHandler.WriteMonitorPort +=new IpcHandler.WriteMonitorPortEventHandler(portMonitor.Write);
	
		this.pipeServer = new PipeServer(ipcHandler);
		//this.pipeServer.ClientConnected += new PipeServer.ClientConnectedEventHandler(this.ClientConnected);
		//this.pipeServer.ReadCompleted += new PipeServer.ReadCompletedEventHandler(this.ReadCompleted);
		//this.pipeServer.WriteCompleted += new PipeServer.WriteCompletedEventHandler(this.WriteCompleted);
		
		this.pipeServer.Start();
		
		while (this.pipeServer.Step())
		{
			// could do some other work here
		}
		
		// fixme: dispose pipeServer?
	}
	
	public void Stop()
	{
		this.pipeServer.Stop();
	}

	/*
	private void ClientConnected(object sender)
	{
	
	}

	private void ReadCompleted(object sender)
	{
		
	}
	
	private void WriteCompleted(object sender)
	{
	
	}
	*/
}

} // namespace PrintJackService
