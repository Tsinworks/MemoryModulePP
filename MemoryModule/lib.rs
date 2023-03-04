
#[repr(transparent)]
pub struct Memmod(*mut usize);

impl Memmod {
    pub fn load(buf: &[u8], name: &str, flags: u32) -> Self {
        unsafe {
            let name = std::ffi::CString::new(name).unwrap();
            Self(LoadLibraryMemoryExA(buf.as_ptr(), buf.len(), name.as_ptr(), name.as_ptr(), flags))
        }
    }
}

impl Drop for Memmod {
    fn drop(&mut self) {
        unsafe {
            FreeLibraryMemory(self.0);
        }
    }
}

#[cfg(windows)]
extern "stdcall" {
    fn LoadLibraryMemoryExA(addr: *const u8, size: usize, base_name: *const i8, full_name: *const i8, flags: u32) -> *mut usize;
    fn LoadLibraryMemoryExW(addr: *const u8, size: usize, base_name: *const u16, full_name: *const u16, flags: u32) -> *mut usize;
    fn FreeLibraryMemory(module: *mut usize);
}