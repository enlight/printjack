namespace PrintJackConfig
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.portsLabel = new System.Windows.Forms.Label();
            this.portListView = new System.Windows.Forms.ListView();
            this.portColumn = new System.Windows.Forms.ColumnHeader();
            this.descriptionColumn = new System.Windows.Forms.ColumnHeader();
            this.printerColumn = new System.Windows.Forms.ColumnHeader();
            this.addPortBtn = new System.Windows.Forms.Button();
            this.configurePortBtn = new System.Windows.Forms.Button();
            this.deletePortBtn = new System.Windows.Forms.Button();
            this.showOnlyPrintJackPortsCheckBox = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // portsLabel
            // 
            this.portsLabel.AutoSize = true;
            this.portsLabel.Location = new System.Drawing.Point(28, 25);
            this.portsLabel.Name = "portsLabel";
            this.portsLabel.Size = new System.Drawing.Size(97, 13);
            this.portsLabel.TabIndex = 1;
            this.portsLabel.Text = "Ports on this server";
            // 
            // portListView
            // 
            this.portListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.portColumn,
            this.descriptionColumn,
            this.printerColumn});
            this.portListView.FullRowSelect = true;
            this.portListView.Location = new System.Drawing.Point(28, 50);
            this.portListView.MultiSelect = false;
            this.portListView.Name = "portListView";
            this.portListView.Size = new System.Drawing.Size(514, 246);
            this.portListView.TabIndex = 2;
            this.portListView.UseCompatibleStateImageBehavior = false;
            this.portListView.View = System.Windows.Forms.View.Details;
            // 
            // portColumn
            // 
            this.portColumn.Text = "Port";
            this.portColumn.Width = 228;
            // 
            // descriptionColumn
            // 
            this.descriptionColumn.Text = "Description";
            this.descriptionColumn.Width = 154;
            // 
            // printerColumn
            // 
            this.printerColumn.Text = "Printer";
            this.printerColumn.Width = 127;
            // 
            // addPortBtn
            // 
            this.addPortBtn.Location = new System.Drawing.Point(30, 304);
            this.addPortBtn.Name = "addPortBtn";
            this.addPortBtn.Size = new System.Drawing.Size(75, 23);
            this.addPortBtn.TabIndex = 3;
            this.addPortBtn.Text = "Add Port";
            this.addPortBtn.UseVisualStyleBackColor = true;
            this.addPortBtn.Click += new System.EventHandler(this.addPortBtn_Click);
            // 
            // configurePortBtn
            // 
            this.configurePortBtn.Location = new System.Drawing.Point(116, 304);
            this.configurePortBtn.Name = "configurePortBtn";
            this.configurePortBtn.Size = new System.Drawing.Size(100, 23);
            this.configurePortBtn.TabIndex = 4;
            this.configurePortBtn.Text = "Configure Port";
            this.configurePortBtn.UseVisualStyleBackColor = true;
            this.configurePortBtn.Click += new System.EventHandler(this.configurePortBtn_Click);
            // 
            // deletePortBtn
            // 
            this.deletePortBtn.Location = new System.Drawing.Point(222, 304);
            this.deletePortBtn.Name = "deletePortBtn";
            this.deletePortBtn.Size = new System.Drawing.Size(75, 23);
            this.deletePortBtn.TabIndex = 5;
            this.deletePortBtn.Text = "Delete Port";
            this.deletePortBtn.UseVisualStyleBackColor = true;
            this.deletePortBtn.Click += new System.EventHandler(this.deletePortBtn_Click);
            // 
            // showOnlyPrintJackPortsCheckBox
            // 
            this.showOnlyPrintJackPortsCheckBox.AutoSize = true;
            this.showOnlyPrintJackPortsCheckBox.Checked = true;
            this.showOnlyPrintJackPortsCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.showOnlyPrintJackPortsCheckBox.Location = new System.Drawing.Point(394, 304);
            this.showOnlyPrintJackPortsCheckBox.Name = "showOnlyPrintJackPortsCheckBox";
            this.showOnlyPrintJackPortsCheckBox.Size = new System.Drawing.Size(148, 17);
            this.showOnlyPrintJackPortsCheckBox.TabIndex = 6;
            this.showOnlyPrintJackPortsCheckBox.Text = "Show only PrintJack ports";
            this.showOnlyPrintJackPortsCheckBox.UseVisualStyleBackColor = true;
            this.showOnlyPrintJackPortsCheckBox.CheckedChanged += new System.EventHandler(this.showOnlyPrintJackPortsCheckBox_CheckedChanged);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(666, 407);
            this.Controls.Add(this.showOnlyPrintJackPortsCheckBox);
            this.Controls.Add(this.deletePortBtn);
            this.Controls.Add(this.configurePortBtn);
            this.Controls.Add(this.addPortBtn);
            this.Controls.Add(this.portListView);
            this.Controls.Add(this.portsLabel);
            this.Name = "MainForm";
            this.Text = "PrintJack Configuration";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label portsLabel;
        private System.Windows.Forms.ListView portListView;
        private System.Windows.Forms.ColumnHeader portColumn;
        private System.Windows.Forms.ColumnHeader descriptionColumn;
        private System.Windows.Forms.ColumnHeader printerColumn;
        private System.Windows.Forms.Button addPortBtn;
        private System.Windows.Forms.Button configurePortBtn;
        private System.Windows.Forms.Button deletePortBtn;
        private System.Windows.Forms.CheckBox showOnlyPrintJackPortsCheckBox;
    }
}

