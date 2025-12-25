use std::{
    ffi::{c_char, CStr, CString},
    fs::{self},
    ptr::{null, null_mut},
    sync::Mutex,
};

use helios_macros::helios_api;
use once_cell::sync::Lazy;

extern crate helios_macros;

pub mod scene;
pub mod stringarray;
pub mod structinterface;

static HELIOS_SCRIPTS: Lazy<Mutex<Vec<Box<dyn scene::EntityBehaviour>>>> = Lazy::new(|| Mutex::new(Vec::new()));

#[no_mangle]
pub extern "C" fn helios_update_all() {
    let mut scripts = HELIOS_SCRIPTS.lock().unwrap();
    for script in scripts.iter_mut() {
        script.on_update();
    }
}

#[no_mangle]
pub extern "C" fn helios_awake() {
    
}

#[macro_export]
macro_rules! helios_api_exports {
    () => {
        #[no_mangle]
        pub extern "C" fn get_api_functions() -> *mut *mut std::ffi::c_char {
            helios_macros::exported_functions!();
            static mut FUNCTIONS: *mut *mut std::ffi::c_char = std::ptr::null_mut();

            unsafe {
                if FUNCTIONS.is_null() {
                    FUNCTIONS = {
                        let mut x = HELIOS_API_FUNCTIONS
                            .map(|s| std::ffi::CString::new(s).unwrap().into_raw())
                            .to_vec();
                        x.push(std::ptr::null_mut());
                        x.as_mut_ptr()
                    };
                }
                return FUNCTIONS;
            }
        }
        #[no_mangle]
        pub extern "C" fn get_api_structs() -> *mut *mut std::ffi::c_char {
            helios_macros::exported_structs!();
            static mut FUNCTIONS: *mut *mut std::ffi::c_char = std::ptr::null_mut();

            unsafe {
                if FUNCTIONS.is_null() {
                    FUNCTIONS = {
                        let mut x = HELIOS_API_STRUCTS
                            .map(|s| std::ffi::CString::new(s).unwrap().into_raw())
                            .to_vec();
                        x.push(std::ptr::null_mut());
                        x.as_mut_ptr()
                    };
                }
                return FUNCTIONS;
            }
        }
    };
}
