use std::fs::{self};


extern crate helios_macros;

mod stringarray;

pub fn add(left: u64, right: u64) -> u64 {
    left + right
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }
}



// lazy_static! {
//     static ref BEHAVIOUR
// }

// static HELIOS_API_STRUCTS: Vec<String> = Vec::new();

extern "C" {
    pub fn test_rust();
}

#[no_mangle]
pub extern "C" fn hello_rust() {
    println!("Hello, World From Rust!");
    unsafe { test_rust() };
}

#[no_mangle]
pub extern "C" fn get_api_functions(path: *const i8) -> *mut stringarray::StringArray {
    // let mut x = helios_globals::HELIOS_API_FUNCTIONS.lock().unwrap().clone();
    let s = unsafe { std::ffi::CStr::from_ptr(path)
        .to_string_lossy()
        .into_owned() };
    println!("Path: {}", s);

    let bytes = fs::read(s).expect("Failed to read file");
    let mut strings = Vec::new();

    // Step 2 & 3: Process each chunk and convert it to a String
    let mut current_string = String::new();
    for byte in bytes {
        if byte == 0 { // Null terminator indicates the end of a string
            strings.push(current_string.clone()); // Push the current string to the vector
            current_string.clear(); // Reset the current string
        } else {
            current_string.push(byte as char); // Append the byte to the current string
        }
    }
    // Don't forget to push the last string if it wasn't terminated by a null byte
    if !current_string.is_empty() {
        strings.push(current_string);
    }

    // let mut x = Vec::new();
    // f.read_to_end(&mut x).expect("Failed to read file");
    
    let mut x = strings;

    x.push(String::from("TEST"));
    // let strarr = stringarray::StringArray::new(HELIOS_API_FUNCTIONS.lock().unwrap().clone());
    let strarr = stringarray::StringArray::new(x);
    Box::into_raw(Box::new(strarr))
}

pub mod scene;