using System.Runtime.InteropServices;

namespace ProjectSelector
{

    public partial class Form1 : Form
    {
        public static string ApplicationFolderName = "HeliosEngine";

        public Form1()
        {
            InitializeComponent();
            UpdateList();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
        }

        private void UpdateList()
        {
            projectsPanel.Controls.Clear();

            List<ProjectData> projects = ProjectManager.GetProjectList();

            for(int i = 0; i < projects.Count; i++)
            {
                ProjectControl projectControl = new(projects[i].name, projects[i].path, () => {
                    UpdateList();
                });
                projectControl.Top = projectControl.Height * i;
                projectsPanel.Controls.Add(projectControl);
            }
        }

        private void new_project_button_Click(object sender, EventArgs e)
        {
            using (New_Project project = new New_Project())
            {
                if(project.ShowDialog() == DialogResult.OK)
                    UpdateList();
            }
        }
    }
}