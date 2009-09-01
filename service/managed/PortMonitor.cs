using System;
using System.Collections.Generic;
using System.Text;
using PrintJackServiceMixed;

namespace PrintJackService {

class PortMonitor
{
	public void AddPort(object sender, AddPortEventArgs e)
	{
        Console.WriteLine("PortMonitor::AddPort");
        e.returnCode = 1;
        e.errorCode = 0;
	}
	
	public void DeletePort(object sender, DeletePortEventArgs e)
	{
        Console.WriteLine("PortMonitor::DeletePort");
        e.returnCode = 1;
        e.errorCode = 0;
	}

    public void OpenPort(object sender, OpenPortEventArgs e)
    {
        Console.WriteLine("PortMonitor::OpenPort");
        e.returnCode = 1;
        e.errorCode = 0;
    }

    public void ClosePort(object sender, ClosePortEventArgs e)
    {
        Console.WriteLine("PortMonitor::ClosePort");
        e.returnCode = 1;
        e.errorCode = 0;
    }

    public void StartDoc(object sender, StartDocEventArgs e)
    {
        Console.WriteLine("PortMonitor::StartDoc");
        e.returnCode = 1;
        e.errorCode = 0;
    }

    public void EndDoc(object sender, EndDocEventArgs e)
    {
        Console.WriteLine("PortMonitor::EndDoc");
        e.returnCode = 1;
        e.errorCode = 0;
    }

    public void Write(object sender, WriteEventArgs e)
    {
        Console.WriteLine("PortMonitor::Write");
        e.returnCode = 1;
        e.errorCode = 0;
    }
}

} // namespace PrintJackService
