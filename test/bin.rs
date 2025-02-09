extern crate winapi;
extern crate memmodpp;
extern crate libc;

fn to_wide_chars(s: &str) -> Vec<u16> {
    use std::ffi::OsStr;
    use std::os::windows::ffi::OsStrExt;
    OsStr::new(s).encode_wide().chain(Some(0).into_iter()).collect::<Vec<_>>()
}

pub fn acquire_anisette_v1() {
    unsafe {
        use winapi::um::libloaderapi::*;

        let flags = LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SAFE_CURRENT_DIRS | LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS;
        let sup_path = to_wide_chars("C:/Program Files (x86)/Common Files/Apple/Apple Application Support");
        let _ret = AddDllDirectory(sup_path.as_ptr());
        let sup_path = to_wide_chars("C:/Program Files (x86)/Common Files/Apple/Internet Services");
        let _ret = AddDllDirectory(sup_path.as_ptr());
        
        // Load libraries
        let objc = LoadLibraryExA(
            "C:/Program Files (x86)/Common Files/Apple/Apple Application Support/objc.dll\0".as_ptr() as _,
            std::ptr::null_mut(),
            flags.clone()
        );
        let cf = LoadLibraryExA(
            "C:/Program Files (x86)/Common Files/Apple/Apple Application Support/CoreFoundation.dll\0".as_ptr() as _,
            std::ptr::null_mut(),
            flags.clone()
        );
        let cfnet = LoadLibraryExA(
            "C:/Program Files (x86)/Common Files/Apple/Apple Application Support/CFNetwork.dll\0".as_ptr() as _,
            std::ptr::null_mut(),
            flags.clone()
        );
        
        winapi::um::libloaderapi::FreeLibrary(cf);
        winapi::um::libloaderapi::FreeLibrary(objc);
    }
}


thread_local! {
    pub static ATTR_TLS: u32 = 0;
}

pub fn acquire_anisette_v2() {
    use memmodpp::Memmod;
    let buf = Vec::new();
    let memmod = Memmod::load(&buf, "iCloud", 0);
}

pub fn acquire_anisette_v3() {
    unsafe {
        use winapi::um::libloaderapi::*;

        let flags = LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SAFE_CURRENT_DIRS | LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS;
        let sup_path = to_wide_chars("C:/Program Files (x86)/i4Tools7/iCloudDll");
        let _ret = AddDllDirectory(sup_path.as_ptr());
        
        // Load libraries
        let objc = LoadLibraryExA(
            "C:/Program Files (x86)/i4Tools7/iCloudDll/objc.dll\0".as_ptr() as _,
            std::ptr::null_mut(),
            flags.clone()
        );
        let cf = LoadLibraryExA(
            "C:/Program Files (x86)/i4Tools7/iCloudDll/CoreFoundation.dll\0".as_ptr() as _,
            std::ptr::null_mut(),
            flags.clone()
        );
        let cfnet = LoadLibraryExA(
            "C:/Program Files (x86)/i4Tools7/iCloudDll/CFNetwork.dll\0".as_ptr() as _,
            std::ptr::null_mut(),
            flags.clone()
        );
        let icloud = LoadLibraryExA(
            "C:/Program Files (x86)/i4Tools7/iCloudDll/iCloud_main.dll\0".as_ptr() as _,
            std::ptr::null_mut(),
            flags.clone()
        );
        // order: objc.dll 
        // CoreFoundation.dll ASL.dll libdispatch.dll libicuuc.dll libicuin.dll icudt62.dll 
        // CFNetwork.dll libxml2.dll SQLite3.dll zlib1.dll pthreadVC2.dll
        // iCloud_main.dll ApplePushService.dll AOSKit.dll JavaScriptCore.dll 
        // msvcp120.dll msvcr120.dll WebKit.dll msvcp100.dll msvcr100.dll Foundation.dll WTF.dll
        // CoreText.dll CoreGraphics.dll libxslt.dll QuartzCore.dll libtidy.dll CoreVideo.dll
        winapi::um::libloaderapi::FreeLibrary(cf);
        winapi::um::libloaderapi::FreeLibrary(objc);
    }
}

fn main() {
    acquire_anisette_v1();
    acquire_anisette_v3();
    acquire_anisette_v2();
}

#[cfg(test)] 
mod tests {
    use super::*;
    
    #[test]
    fn test_acquire_anisette() {
        acquire_anisette_v1();
        acquire_anisette_v2();
    }
}