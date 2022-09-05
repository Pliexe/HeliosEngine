using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ProjectSelector
{
    public partial class New_Project : Form
    {
        string project_dir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "Helios Projects");

        public New_Project()
        {
            InitializeComponent();
            textBox_project_directory.Text = project_dir;
        }

        public string GetDirectory()
        {
            return project_dir;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if(textBox_project_name.Text == "")
            {
                MessageBox.Show("Project name is empty!", "Failed to create project!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                DialogResult = DialogResult.None;
                return;
            }

            if(ProjectManager.GetProjectList().Any(d => d.name == textBox_project_name.Text))
            {
                MessageBox.Show("Project with that name already exists!", "Failed to create project!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                DialogResult = DialogResult.None;
                return;
            }

            try
            {
                Directory.CreateDirectory(project_dir);

                ProjectManager.CreateProject(textBox_project_name.Text, textBox_project_directory.Text);
                Close();
            }
            catch (PathTooLongException)
            {
                DialogResult = DialogResult.None;
                MessageBox.Show("The path is too long!", "Failed to create project!", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (UnauthorizedAccessException)
            {
                DialogResult = DialogResult.None;
                MessageBox.Show("Unauthorized access!", "Failed to create project!", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (DirectoryNotFoundException)
            {
                DialogResult = DialogResult.None;
                MessageBox.Show("The path given is invalid!", "Failed to create project!", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            using(FolderBrowserDialog dialog = new FolderBrowserDialog())
            {
                string path = project_dir;
                if(!Directory.Exists(path))
                    Directory.CreateDirectory(path);

                dialog.InitialDirectory = path;
                if(dialog.ShowDialog().Equals(DialogResult.OK))
                {
                    textBox_project_directory.Text = Path.Combine(project_dir, textBox_project_name.Text);
                }
            }
        }


        private void button3_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void textBox_project_directory_Validating(object sender, CancelEventArgs e)
        {
            textBox_project_directory.Text = Path.Combine(project_dir, textBox_project_name.Text);
        }

        private void textBox_project_name_TextChanged(object sender, EventArgs e)
        {
            textBox_project_directory.Text = Path.Combine(project_dir, textBox_project_name.Text);
        }
    }
}
