using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.ServiceProcess;
using System.Text;
using System.Threading;

namespace PrintJackService
{
	public partial class PortMonitorService : ServiceBase
	{
		private Thread serverThread;
		private ServerThreadState serverThreadState;
	
		public PortMonitorService()
		{
			InitializeComponent();
		}

		protected override void OnStart(string[] args)
		{
			this.serverThreadState = new ServerThreadState();
			this.serverThread = new Thread(new ThreadStart(this.serverThreadState.Start));
			this.serverThread.Start();
		}

		protected override void OnStop()
		{
			// request the pipe server thread to terminate
			this.serverThreadState.Stop();
			// wait for it to terminate
			this.serverThread.Join();
			// cleanup
			this.serverThread = null;
			//this.serverThreadState.Dispose();
		}
	}
}
