namespace ProjectSelector
{
    partial class ProjectControl
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.project_name = new System.Windows.Forms.Label();
            this.project_path = new System.Windows.Forms.Label();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.removeFromListToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // project_name
            // 
            this.project_name.AutoSize = true;
            this.project_name.Font = new System.Drawing.Font("Segoe UI", 15F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point);
            this.project_name.Location = new System.Drawing.Point(15, 19);
            this.project_name.Name = "project_name";
            this.project_name.Size = new System.Drawing.Size(216, 28);
            this.project_name.TabIndex = 0;
            this.project_name.Text = "Sample Project Name";
            this.project_name.Click += new System.EventHandler(this.project_name_Click);
            // 
            // project_path
            // 
            this.project_path.AutoSize = true;
            this.project_path.Location = new System.Drawing.Point(15, 47);
            this.project_path.Name = "project_path";
            this.project_path.Size = new System.Drawing.Size(48, 15);
            this.project_path.TabIndex = 1;
            this.project_path.Text = "C:/Deez";
            this.project_path.Click += new System.EventHandler(this.project_path_Click);
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.removeFromListToolStripMenuItem});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(165, 26);
            // 
            // removeFromListToolStripMenuItem
            // 
            this.removeFromListToolStripMenuItem.Name = "removeFromListToolStripMenuItem";
            this.removeFromListToolStripMenuItem.Size = new System.Drawing.Size(164, 22);
            this.removeFromListToolStripMenuItem.Text = "Remove from list";
            this.removeFromListToolStripMenuItem.Click += new System.EventHandler(this.removeFromListToolStripMenuItem_Click);
            // 
            // ProjectControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ContextMenuStrip = this.contextMenuStrip1;
            this.Controls.Add(this.project_path);
            this.Controls.Add(this.project_name);
            this.Name = "ProjectControl";
            this.Size = new System.Drawing.Size(349, 80);
            this.Load += new System.EventHandler(this.ProjectControl_Load);
            this.MouseClick += new System.Windows.Forms.MouseEventHandler(this.ProjectControl_MouseClick);
            this.contextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private Label project_name;
        private Label project_path;
        private ContextMenuStrip contextMenuStrip1;
        private ToolStripMenuItem removeFromListToolStripMenuItem;
    }
}
