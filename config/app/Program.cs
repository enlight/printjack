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
using System.Windows.Forms;

namespace PrintJackConfig
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            
            switch (args.Length)
            {
                case 0:
                    Application.Run(new MainForm());
                    break;

                case 1:
                    if ("-add" == args[0])
                        Application.Run(new AddPortForm());
                    break;

                case 2:
                    if ("-configure" == args[0])
                        Application.Run(new ConfigurePortForm(args[1]));
                    break;
            }
        }
    }
}