using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ProjectSelector
{
    public partial class ProjectControl : UserControl
    {
        bool is_open = false;
        Action onRefresh;

        public ProjectControl(string name, string path, Action onRefresh)
        {
            this.onRefresh = onRefresh;
            InitializeComponent();
            project_name.Text = name;
            project_path.Text = path;
        }

        private void ProjectControl_Load(object sender, EventArgs e)
        {

        }

        enum InvalidReason
        {
            InvalidPath = -1,
            InvalidProjectStructure = -2,
            InvalidReadAndWritePermissions = -3,
        }

        void StartProgram()
        {
            int result;
            if ((result = ProjectManager.IsValidProject(project_path.Text)) == 0)
                Process.Start("HeliosEditor.exe", $"\"{project_path.Text}\"");
            else
            {
                switch((InvalidReason)result)
                {
                    case InvalidReason.InvalidReadAndWritePermissions:
                        MessageBox.Show("Missing Read or Write permissions!", "Unable to open project!", MessageBoxButtons.OK, MessageBoxIcon.Stop);
                        break;
                    default:
                        MessageBox.Show("The Project is Invalid!", "Unable to open project!", MessageBoxButtons.OK, MessageBoxIcon.Stop);
                        break;
                }
            }
        }
        private void removeFromListToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ProjectManager.RemoveFromList(project_path.Text);
            onRefresh();
        }

        private void ProjectControl_MouseClick(object sender, MouseEventArgs e)
        {
            StartProgram();
        }

        private void project_name_Click(object sender, EventArgs e)
        {
            StartProgram();
        }

        private void project_path_Click(object sender, EventArgs e)
        {
            StartProgram();
        }
    }
}
