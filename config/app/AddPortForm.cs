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

namespace PrintJackConfig
{
    public partial class AddPortForm : Form
    {
        public AddPortForm()
        {
            InitializeComponent();
        }

        public string PortName
        {
            get 
            { 
                return this.portNameTextBox.Text; 
            }
        }

        private void okBtn_Click(object sender, EventArgs e)
        {
            if (0 == this.PortName.Length)
            {
                MessageBox.Show(
                    this, 
                    "A port name must be entered.", 
                    "Error",
                    MessageBoxButtons.OK, MessageBoxIcon.Error
                );
                return;
            }

            try
            {
                PortManager.AddPrintJackPort(this.PortName);
            }
            catch (PortAlreadyExistsException)
            {
				MessageBox.Show(
					this,
					"A port with this name already exists.",
					"Error",
					MessageBoxButtons.OK, MessageBoxIcon.Error
				);
				return;
            }
            
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void cancelBtn_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}