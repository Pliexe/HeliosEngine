// fn main() {
//     println!("cargo:rustc-link-lib=dylib=HeliosEngine");
// }

// Project/BuildSystems/Rust/rust/src/build.rs

use std::env;
use std::fmt::format;
use std::fs::{self, File};
use std::io::Write;
use std::path::{Path, PathBuf};

fn main() {
    println!("cargo:rustc-link-lib=dylib=HeliosEngine");

    let out_dir = env::var_os("OUT_DIR").unwrap();

    println!("cargo:warning=outdir:{:?}", out_dir);

    let assets_dir = PathBuf::from("../../Assets");
    // let assets_mod_dir = PathBuf::from("../../Assets");
    let dest_path = PathBuf::from(out_dir).join("generated_mods.rs");

    // let dest_path = PathBuf::from("src/bindings.rs");

    let mut f = File::create(&dest_path).expect("Failed to create file");

    // Recursive function to process directories
    fn process_directory(path: &Path, f: &mut File) {
        for entry in fs::read_dir(path).expect("Failed to read directory") {
            let entry = entry.expect("Failed to read entry");
            let path = entry.path();

            if path.is_file() && path.extension().map_or(false, |ext| ext == "rs") {
                let mod_name = format!("assets_{}_{}", path.strip_prefix(path.parent().unwrap()).unwrap().parent().unwrap().display().to_string(), path.file_stem().unwrap().to_str().unwrap());
                let relative_path = format!("../../../../../{}", path.to_str().unwrap().replace("\\", "/"));
                // let relative_path = format!("../../../../../{}", path.to_str().unwrap().replace("\\", "/"));
                writeln!(f, "#[path = \"{}\"]", relative_path).expect("Failed to write to file");
                writeln!(f, "pub mod {};", mod_name).expect("Failed to write to file");
            } else if path.is_dir() {
                process_directory(&path, f);
            }
        }
    }

    process_directory(&assets_dir, &mut f);

    println!("cargo:rerun-if-changed=../../Assets/*");
}
