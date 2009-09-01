using System;
using System.Collections;
using System.Configuration.Install;
using System.ServiceProcess;
using System.ComponentModel;

namespace PrintJackService {
/// <summary>
/// Helper class for installing debug versions of the service using 
/// InstallUtil.exe without creating an MSI installer to do it.
/// </summary>
[RunInstaller(true)]
public partial class DebugServiceInstaller : Installer
{
    private ServiceInstaller serviceInstaller;
    private ServiceProcessInstaller serviceProcessInstaller;

    public DebugServiceInstaller()
    {
        serviceInstaller = new ServiceInstaller();
        serviceProcessInstaller = new ServiceProcessInstaller();

        serviceProcessInstaller.Account = ServiceAccount.LocalSystem;

        serviceInstaller.StartType = ServiceStartMode.Automatic;

        serviceInstaller.ServiceName = "PrintJack";

        Installers.Add(serviceInstaller);
        Installers.Add(serviceProcessInstaller);
    }
}

} // namespace PrintJackService