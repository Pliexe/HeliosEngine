namespace ProjectSelector
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.projectsPanel = new System.Windows.Forms.Panel();
            this.new_project_button = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // projectsPanel
            // 
            this.projectsPanel.AutoScroll = true;
            this.projectsPanel.Location = new System.Drawing.Point(302, 60);
            this.projectsPanel.Name = "projectsPanel";
            this.projectsPanel.Size = new System.Drawing.Size(371, 339);
            this.projectsPanel.TabIndex = 0;
            // 
            // new_project_button
            // 
            this.new_project_button.Location = new System.Drawing.Point(56, 118);
            this.new_project_button.Name = "new_project_button";
            this.new_project_button.Size = new System.Drawing.Size(75, 23);
            this.new_project_button.TabIndex = 1;
            this.new_project_button.Text = "New Project";
            this.new_project_button.UseVisualStyleBackColor = true;
            this.new_project_button.Click += new System.EventHandler(this.new_project_button_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.new_project_button);
            this.Controls.Add(this.projectsPanel);
            this.Name = "Form1";
            this.Text = "Helios Projets";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private Panel projectsPanel;
        private Button new_project_button;
    }
}