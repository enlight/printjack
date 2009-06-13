//-----------------------------------------------------------------------------
// Copyright 2009 Vadim Macagon
// 
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
// 
// http://www.apache.org/licenses/LICENSE-2.0 
// 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.
//-----------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Microsoft.Win32;
using PrintJackConfigManaged;

namespace PrintJackConfig {

public partial class ConfigurePortForm : Form
{
    private string portName;

    public ConfigurePortForm(string portName)
    {
        InitializeComponent();
        
        this.portName = portName;

        this.UpdateFormFromPortConfig();
    }

    private void okBtn_Click(object sender, EventArgs e)
    {
        this.UpdatePortConfigFromForm();
        this.Close();
    }

    private void cancelBtn_Click(object sender, EventArgs e)
    {
        this.Close();
    }

    private void appPathBtn_Click(object sender, EventArgs e)
    {
        using (OpenFileDialog dlg = new OpenFileDialog())
        {
            if (DialogResult.OK == dlg.ShowDialog())
            {
                this.appPathTextBox.Text = dlg.FileName;
            }
        }
    }

    private void UpdateFormFromPortConfig()
    {
        // direct access to the registry like this won't work for port monitors 
        // on clustered servers, however since it only makes sense to create
        // local PrintJack ports this will do I think (for now!)... 
        // the proper way to do this though is to route configuration changes 
        // through the port monitor server dll using XcvData(), then the server
        // dll can write the config to the registry via the MONITORREG instance
        string portKeyPath = "SYSTEM\\CurrentControlSet\\Control\\Print\\Monitors\\PrintJack\\Ports\\" + this.portName;
	    RegistryKey portKey = Registry.LocalMachine.OpenSubKey(portKeyPath);
	    if (portKey != null)
	    {
		    this.appPathTextBox.Text = (string)portKey.GetValue("AppPath");
		    this.appArgsTextBox.Text = (string)portKey.GetValue("AppArgs");
		    portKey.Close();
	    }
    }

    private void UpdatePortConfigFromForm()
    {
        string portKeyPath = "SYSTEM\\CurrentControlSet\\Control\\Print\\Monitors\\PrintJack\\Ports\\" + this.portName;
	    RegistryKey portKey = Registry.LocalMachine.OpenSubKey(portKeyPath, true);
	    if (portKey != null)
	    {
		    portKey.SetValue("AppPath", this.appPathTextBox.Text, RegistryValueKind.String);
		    portKey.SetValue("AppArgs", this.appArgsTextBox.Text, RegistryValueKind.String);
		    portKey.Close();
	    }
    }
}

} // namespace PrintJackConfig
