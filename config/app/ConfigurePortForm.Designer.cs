namespace PrintJackConfig
{
    partial class ConfigurePortForm
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
            this.okBtn = new System.Windows.Forms.Button();
            this.cancelBtn = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.appPathLabel = new System.Windows.Forms.Label();
            this.appArgsLabel = new System.Windows.Forms.Label();
            this.appPathTextBox = new System.Windows.Forms.TextBox();
            this.appArgsTextBox = new System.Windows.Forms.TextBox();
            this.appPathBtn = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // okBtn
            // 
            this.okBtn.Location = new System.Drawing.Point(239, 102);
            this.okBtn.Name = "okBtn";
            this.okBtn.Size = new System.Drawing.Size(75, 23);
            this.okBtn.TabIndex = 0;
            this.okBtn.Text = "OK";
            this.okBtn.UseVisualStyleBackColor = true;
            this.okBtn.Click += new System.EventHandler(this.okBtn_Click);
            // 
            // cancelBtn
            // 
            this.cancelBtn.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelBtn.Location = new System.Drawing.Point(320, 102);
            this.cancelBtn.Name = "cancelBtn";
            this.cancelBtn.Size = new System.Drawing.Size(75, 23);
            this.cancelBtn.TabIndex = 1;
            this.cancelBtn.Text = "Cancel";
            this.cancelBtn.UseVisualStyleBackColor = true;
            this.cancelBtn.Click += new System.EventHandler(this.cancelBtn_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.appPathBtn);
            this.groupBox1.Controls.Add(this.appArgsTextBox);
            this.groupBox1.Controls.Add(this.appPathTextBox);
            this.groupBox1.Controls.Add(this.appArgsLabel);
            this.groupBox1.Controls.Add(this.appPathLabel);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(384, 84);
            this.groupBox1.TabIndex = 2;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Application that will process print data";
            // 
            // appPathLabel
            // 
            this.appPathLabel.AutoSize = true;
            this.appPathLabel.Location = new System.Drawing.Point(6, 28);
            this.appPathLabel.Name = "appPathLabel";
            this.appPathLabel.Size = new System.Drawing.Size(29, 13);
            this.appPathLabel.TabIndex = 0;
            this.appPathLabel.Text = "Path";
            // 
            // appArgsLabel
            // 
            this.appArgsLabel.AutoSize = true;
            this.appArgsLabel.Location = new System.Drawing.Point(6, 53);
            this.appArgsLabel.Name = "appArgsLabel";
            this.appArgsLabel.Size = new System.Drawing.Size(57, 13);
            this.appArgsLabel.TabIndex = 1;
            this.appArgsLabel.Text = "Arguments";
            // 
            // appPathTextBox
            // 
            this.appPathTextBox.Location = new System.Drawing.Point(70, 25);
            this.appPathTextBox.Name = "appPathTextBox";
            this.appPathTextBox.Size = new System.Drawing.Size(266, 20);
            this.appPathTextBox.TabIndex = 2;
            // 
            // appArgsTextBox
            // 
            this.appArgsTextBox.Location = new System.Drawing.Point(70, 50);
            this.appArgsTextBox.Name = "appArgsTextBox";
            this.appArgsTextBox.Size = new System.Drawing.Size(266, 20);
            this.appArgsTextBox.TabIndex = 3;
            // 
            // appPathBtn
            // 
            this.appPathBtn.Font = new System.Drawing.Font("Arial Black", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.appPathBtn.Location = new System.Drawing.Point(342, 23);
            this.appPathBtn.Name = "appPathBtn";
            this.appPathBtn.Size = new System.Drawing.Size(28, 23);
            this.appPathBtn.TabIndex = 4;
            this.appPathBtn.Text = "...";
            this.appPathBtn.UseVisualStyleBackColor = true;
            this.appPathBtn.Click += new System.EventHandler(this.appPathBtn_Click);
            // 
            // ConfigurePortForm
            // 
            this.AcceptButton = this.okBtn;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cancelBtn;
            this.ClientSize = new System.Drawing.Size(408, 136);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.cancelBtn);
            this.Controls.Add(this.okBtn);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.Name = "ConfigurePortForm";
            this.Text = "Configure PrintJack Port";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button okBtn;
        private System.Windows.Forms.Button cancelBtn;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label appArgsLabel;
        private System.Windows.Forms.Label appPathLabel;
        private System.Windows.Forms.TextBox appArgsTextBox;
        private System.Windows.Forms.TextBox appPathTextBox;
        private System.Windows.Forms.Button appPathBtn;
    }
}