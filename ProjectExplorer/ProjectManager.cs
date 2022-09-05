using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace ProjectSelector
{
    class ProjectData
    {
        public string name;
        public string path;
    }

    public class ProjectNameExceedException : Exception
    {
        public ProjectNameExceedException() : base("The name exceeded " + ProjectManager.GetMaxProjectName() + " characters!") { }
    }


    internal class ProjectManager
    {
        const string ProjectManagerDll = "ProjectManager.dll";

        [DllImport(ProjectManagerDll, CallingConvention = CallingConvention.Cdecl)]
        public static extern int CreateNewProject([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport(ProjectManagerDll, CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetMaxProjectName();
        
        [DllImport(ProjectManagerDll, CallingConvention = CallingConvention.Cdecl)]
        public static extern int IsValidProject(string path);

        static string ProgramDataPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), Form1.ApplicationFolderName);
        static string ProjectListFileLocation = Path.Combine(ProgramDataPath, "ProjectList");

        public static List<ProjectData> GetProjectList()
        {
            List<ProjectData> projects = new();

            if (!Directory.Exists(ProjectManager.ProgramDataPath)) return projects;
             
            StreamReader file = new(ProjectListFileLocation);

            string? line;

            while((line = file.ReadLine()) != null)
            {
                projects.Add(new ProjectData
                {
                    name = Path.GetFileName(line),
                    path = line
                });
            }
            

            file.Close();
            return projects;
        }

        public static void RemoveFromList(string path)
        {
            List<ProjectData> projects = GetProjectList();

            if (!Directory.Exists(ProjectManager.ProgramDataPath))
                Directory.CreateDirectory(ProjectManager.ProgramDataPath);

            StreamWriter file = new(ProjectListFileLocation);

            foreach (ProjectData proj in projects)
                if(path != proj.path)
                    file.WriteLine(proj.path);

            file.Close();
        }

        public static void CreateProject(string name, string project_dir)
        {
            string project_path = Path.Combine(name, project_dir);

            if (!Directory.Exists(ProjectManager.ProgramDataPath))
                Directory.CreateDirectory(ProjectManager.ProgramDataPath);

            StreamWriter file = new(ProjectListFileLocation, true);

            file.WriteLine(Path.Combine(project_path));

            file.Close();

            CreateNewProject(project_dir);
        }

        public static void OpeneExistingProject(string project_dir)
        {
            
        }
    }
}
