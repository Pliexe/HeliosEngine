// lib.rs or main.rs
use std::ffi::CString;
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn create_string_array() -> *mut StringArray {
    Box::into_raw(Box::new(StringArray::new(Vec::new())))
}

#[no_mangle]
pub extern "C" fn destroy_string_array(array: *mut StringArray) {
    unsafe {
        array.as_mut().unwrap().destroy();
    }
}

#[repr(C)]
pub struct StringArray {
    pub items: *mut *mut c_char,
    pub count: usize,
}

impl StringArray {
    pub fn new(items: Vec<String>) -> Self {
        let count = items.len(); // store the length here
        let mut ptr = Vec::new();
        for item in items {
            let s = CString::new(item).unwrap();
            ptr.push(s.into_raw());
        }

        StringArray {
            items: ptr.as_mut_ptr(),
            count, // use the stored length here
        }
    }

    // Destructor to free memory in C++
    pub fn destroy(&self) {
        for i in 0..self.count {
            unsafe {
                let _ = CString::from_raw(*self.items.offset(i as isize));
            }
        }
        unsafe {
            std::ptr::drop_in_place(self.items as *mut _);
        }    
    }
}
