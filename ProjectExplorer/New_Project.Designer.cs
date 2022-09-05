namespace ProjectSelector
{
    partial class New_Project
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
            this.textBox_project_name = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.create_button = new System.Windows.Forms.Button();
            this.textBox_project_directory = new System.Windows.Forms.TextBox();
            this.open_explorer_button = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.cancel_button = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // textBox_project_name
            // 
            this.textBox_project_name.Location = new System.Drawing.Point(45, 86);
            this.textBox_project_name.Name = "textBox_project_name";
            this.textBox_project_name.Size = new System.Drawing.Size(302, 23);
            this.textBox_project_name.TabIndex = 0;
            this.textBox_project_name.TextChanged += new System.EventHandler(this.textBox_project_name_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(45, 68);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(79, 15);
            this.label1.TabIndex = 1;
            this.label1.Text = "Project Name";
            // 
            // create_button
            // 
            this.create_button.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.create_button.Location = new System.Drawing.Point(49, 187);
            this.create_button.Name = "create_button";
            this.create_button.Size = new System.Drawing.Size(75, 23);
            this.create_button.TabIndex = 2;
            this.create_button.Text = "Create";
            this.create_button.UseVisualStyleBackColor = true;
            this.create_button.Click += new System.EventHandler(this.button1_Click);
            // 
            // textBox_project_directory
            // 
            this.textBox_project_directory.Location = new System.Drawing.Point(45, 141);
            this.textBox_project_directory.Name = "textBox_project_directory";
            this.textBox_project_directory.Size = new System.Drawing.Size(268, 23);
            this.textBox_project_directory.TabIndex = 3;
            this.textBox_project_directory.Validating += new System.ComponentModel.CancelEventHandler(this.textBox_project_directory_Validating);
            // 
            // open_explorer_button
            // 
            this.open_explorer_button.Location = new System.Drawing.Point(319, 140);
            this.open_explorer_button.Name = "open_explorer_button";
            this.open_explorer_button.Size = new System.Drawing.Size(28, 23);
            this.open_explorer_button.TabIndex = 4;
            this.open_explorer_button.Text = "...";
            this.open_explorer_button.UseVisualStyleBackColor = true;
            this.open_explorer_button.Click += new System.EventHandler(this.button2_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(45, 123);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(95, 15);
            this.label2.TabIndex = 5;
            this.label2.Text = "Project Directory";
            // 
            // cancel_button
            // 
            this.cancel_button.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancel_button.Location = new System.Drawing.Point(238, 187);
            this.cancel_button.Name = "cancel_button";
            this.cancel_button.Size = new System.Drawing.Size(75, 23);
            this.cancel_button.TabIndex = 6;
            this.cancel_button.Text = "Cancel";
            this.cancel_button.UseVisualStyleBackColor = true;
            this.cancel_button.Click += new System.EventHandler(this.button3_Click);
            // 
            // New_Project
            // 
            this.AcceptButton = this.create_button;
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cancel_button;
            this.ClientSize = new System.Drawing.Size(398, 249);
            this.Controls.Add(this.cancel_button);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.open_explorer_button);
            this.Controls.Add(this.textBox_project_directory);
            this.Controls.Add(this.create_button);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.textBox_project_name);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "New_Project";
            this.Text = "New Project";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private TextBox textBox_project_name;
        private Label label1;
        private Button create_button;
        private TextBox textBox_project_directory;
        private Button open_explorer_button;
        private Label label2;
        private Button cancel_button;
    }
}