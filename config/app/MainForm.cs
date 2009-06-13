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
using PrintJackConfigManaged;

namespace PrintJackConfig {

public partial class MainForm : Form
{
    public MainForm()
    {
        InitializeComponent();
    }

    private void MainForm_Load(object sender, EventArgs e)
    {
        this.ReloadPorts();
    }

    private void ReloadPorts()
    {
        this.portListView.Items.Clear();
        // get the port list
        List<PortInfo> ports = PortManager.GetPorts();
        // populate the listview
        if (ports != null)
        {
            foreach (PortInfo port in ports)
            {
                if (this.showOnlyPrintJackPortsCheckBox.Checked)
                    if (port.MonitorName != "PrintJack")
                        continue;
                    
                ListViewItem item = new ListViewItem(port.PortName);
                item.SubItems.Add(port.PortDescription);
                item.SubItems.Add(port.PrinterName);
                this.portListView.Items.Add(item);
            }
        }
    }

    private void addPortBtn_Click(object sender, EventArgs e)
    {
        using (AddPortForm form = new AddPortForm())
        {
            if (DialogResult.OK == form.ShowDialog())
                this.ReloadPorts();
        }
    }

    private void deletePortBtn_Click(object sender, EventArgs e)
    {
        if (1 != this.portListView.SelectedItems.Count)
            return;
        
        PortManager.DeletePrintJackPort(this.portListView.SelectedItems[0].Text);
        this.ReloadPorts();
    }

    private void configurePortBtn_Click(object sender, EventArgs e)
    {
        if (1 != this.portListView.SelectedItems.Count)
            return;
        
        using (ConfigurePortForm form = new ConfigurePortForm(this.portListView.SelectedItems[0].Text))
        {
            form.ShowDialog();
        }
    }

    private void showOnlyPrintJackPortsCheckBox_CheckedChanged(object sender, EventArgs e)
    {
        this.ReloadPorts();
    }
}

} // namespace PrintJackConfig
