#[repr(C)]
pub struct StructFunctionInterface {
    name: *mut std::ffi::c_char,
    ptr: *mut std::ffi::c_void,
    parameters: *mut *mut std::ffi::c_char,
    parameters_count: i32,
    return_type: *mut std::ffi::c_char
}

#[repr(C)]
pub struct StructInterface {
    functions: *mut *mut StructFunctionInterface,
    functions_count: i32
}