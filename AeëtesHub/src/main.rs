use std::fs;

use directories_next::UserDirs;
use eframe::egui;

struct AeetesLauncher
{
    projects: Vec<String>,
    selected: Option<usize>,
    panel_open: bool,
}

impl AeetesLauncher
{
    fn new(_cc: &eframe::CreationContext<'_>) -> Self {

        let mut test: Vec<String> = vec![];

        // vec!["Project1".into(), "Project2".into()]

        test.push("Project1".into());
        test.push("Project2".into());

        if let Some(user_dirs) = UserDirs::new()
        {
            if let Some(documents) = user_dirs.document_dir()
            {
                if documents.exists() {
                    let path = documents.join("Helios Projects");
                    if path.exists() {
                        for entry in fs::read_dir(&path).expect("Could not read projects folder") {
                        if let Ok(entry) = entry {
                            let path = entry.path();
                            if path.is_dir() {
                                println!("Project folder: {}", path.display());
                                if let Some(x) = path.into_os_string().to_str()
                                {
                                    test.push(x.into());
                                }
                            }
                        }
                    }
                    }
                }
            }            
        }

        Self {
            projects: test,
            selected: None,
            panel_open: false
        }
    }
}


impl eframe::App for AeetesLauncher
{
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame)
    {
        egui::CentralPanel::default().show(ctx, |ui| {
            ui.heading("Aeëtes Launcher");
            ui.separator();

            ui.horizontal(|ui| {
                ui.vertical(|ui| {
                    ui.label("Projects: ");
                    for (i, project) in self.projects.iter().enumerate() {
                        if ui.selectable_label(self.selected == Some(i), project).clicked() {
                            self.selected = Some(i);
                            self.panel_open = true;
                        }
                    }
                });

                if self.panel_open {
                    if let Some(idx) = self.selected
                    {
                        ui.vertical( |ui| {
                            ui.group(|ui| {
                                ui.label(format!("Selected: {}", self.projects[idx]));
                                if ui.button("Open").clicked() {
                                    println!("User pressed project: {}", self.projects[idx]);
                                }
                                if ui.button("Run").clicked() {
                                    println!("User pressed run: {}", self.projects[idx]);
                                }
                            });
                        });
                    }
                }
            });
        });

        // if show_popup {
        //     egui::Window::new("Message")
        //         .collapsible(false)
        //         .resizable(false)
        //         .show(ctx, |ui| {
        //             ui.label("This is a message!");
        //             if ui.button("OK").clicked() {
        //                 show_popup = false;
        //             }
        //         });
        // }
        
        ctx.request_repaint();
    }
}

fn main() {
    println!("Hello, world!");

    let native_options = eframe::NativeOptions::default();

    // let options = eframe::NativeOptions {
    //     viewport: Some(Box::new(|wb| {
    //         wb.with_title("Aeëtes Launcher")
    //         .with_inner_size(winit::dpi::PhysicalSize::new(600, 400))
    //     })),
    //     vsync: true,
    //     ..Default::default()
    // };

    let result = eframe::run_native(
        "Aeëtes Launcher",
        native_options,
        Box::new(|cc| Ok(Box::new(AeetesLauncher::new(cc))))
    );

    if result.is_err()
    {
        println!("Error starting application!\n\t{}", "idk how to display");
    }
}
