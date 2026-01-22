use std::ffi::{CStr, CString};
use std::os::raw::{c_char, c_int, c_float, c_void};
use std::path::PathBuf;
use std::sync::{Arc, Mutex};
use std::thread;
use std::ptr;

mod core;
use core::*;

// FFI-safe device structure
#[repr(C)]
pub struct CUsbDevice {
    pub path: *mut c_char,
    pub size: *mut c_char,
    pub size_bytes: u64,
    pub removable: bool,
    pub mounted: bool,
    pub is_system_disk: bool,
    pub mount_point_count: usize,
    pub mount_points: *mut *mut c_char,
}

// FFI-safe device list
#[repr(C)]
pub struct CDeviceList {
    pub devices: *mut CUsbDevice,
    pub count: usize,
}

// Operation handle for async operations (opaque pointer)
pub struct CFlashOperation {
    progress: Arc<Mutex<f32>>,
    status: Arc<Mutex<String>>,
    bytes_written: Arc<Mutex<u64>>,
    verify_progress: Arc<Mutex<f32>>,
    is_running: Arc<Mutex<bool>>,
    error: Arc<Mutex<Option<String>>>,
}

// Progress callback type
pub type ProgressCallback = extern "C" fn(progress: c_float, user_data: *mut c_void);

/// Initialize the library (currently no-op, but useful for future initialization)
#[no_mangle]
pub extern "C" fn flux_init() -> c_int {
    0 // Success
}

/// Cleanup the library (currently no-op)
#[no_mangle]
pub extern "C" fn flux_cleanup() {
    // No-op for now
}

/// List all removable USB devices
#[no_mangle]
pub extern "C" fn flux_list_devices() -> *mut CDeviceList {
    match list_usb_devices() {
        Ok(devices) => {
            let c_devices: Vec<CUsbDevice> = devices.into_iter().map(|dev| {
                let path = CString::new(dev.path).unwrap();
                let size = CString::new(dev.size).unwrap();
                
                let mount_points: Vec<*mut c_char> = dev.mount_points
                    .into_iter()
                    .map(|mp| CString::new(mp).unwrap().into_raw())
                    .collect();
                
                let mount_point_count = mount_points.len();
                let mount_points_ptr = if mount_point_count > 0 {
                    let mut vec = mount_points;
                    let ptr = vec.as_mut_ptr();
                    std::mem::forget(vec);
                    ptr
                } else {
                    ptr::null_mut()
                };
                
                CUsbDevice {
                    path: path.into_raw(),
                    size: size.into_raw(),
                    size_bytes: dev.size_bytes,
                    removable: dev.removable,
                    mounted: dev.mounted,
                    is_system_disk: dev.is_system_disk,
                    mount_point_count,
                    mount_points: mount_points_ptr,
                }
            }).collect();
            
            let count = c_devices.len();
            let mut boxed = c_devices.into_boxed_slice();
            let ptr = boxed.as_mut_ptr();
            std::mem::forget(boxed);
            
            Box::into_raw(Box::new(CDeviceList {
                devices: ptr,
                count,
            }))
        }
        Err(_) => ptr::null_mut(),
    }
}

/// Free a device list returned by flux_list_devices
#[no_mangle]
pub extern "C" fn flux_free_device_list(list: *mut CDeviceList) {
    if list.is_null() {
        return;
    }
    
    unsafe {
        let list = Box::from_raw(list);
        let devices = Vec::from_raw_parts(list.devices, list.count, list.count);
        
        for device in devices {
            let _ = CString::from_raw(device.path);
            let _ = CString::from_raw(device.size);
            
            if !device.mount_points.is_null() {
                let mount_points = Vec::from_raw_parts(
                    device.mount_points,
                    device.mount_point_count,
                    device.mount_point_count
                );
                for mp in mount_points {
                    let _ = CString::from_raw(mp);
                }
            }
        }
    }
}

/// Start a flash operation (async)
#[no_mangle]
pub extern "C" fn flux_start_flash(
    image_path: *const c_char,
    device_path: *const c_char,
) -> *mut CFlashOperation {
    if image_path.is_null() || device_path.is_null() {
        return ptr::null_mut();
    }
    
    let image_path = unsafe { CStr::from_ptr(image_path) }.to_string_lossy().into_owned();
    let device_path = unsafe { CStr::from_ptr(device_path) }.to_string_lossy().into_owned();
    
    let operation = CFlashOperation {
        progress: Arc::new(Mutex::new(0.0)),
        status: Arc::new(Mutex::new("Initializing...".to_string())),
        bytes_written: Arc::new(Mutex::new(0)),
        verify_progress: Arc::new(Mutex::new(0.0)),
        is_running: Arc::new(Mutex::new(true)),
        error: Arc::new(Mutex::new(None)),
    };
    
    let progress = operation.progress.clone();
    let status = operation.status.clone();
    let bytes_written = operation.bytes_written.clone();
    let verify_progress = operation.verify_progress.clone();
    let is_running = operation.is_running.clone();
    let error = operation.error.clone();
    
    thread::spawn(move || {
        let image_pb = PathBuf::from(image_path);
        
        // Get mount points
        let mount_points = match list_usb_devices() {
            Ok(devices) => {
                devices.iter()
                    .find(|d| d.path == device_path)
                    .map(|d| d.mount_points.clone())
                    .unwrap_or_default()
            }
            Err(_) => Vec::new(),
        };
        
        // Flash phase
        match flash_image(&image_pb, &device_path, progress.clone(), status.clone(), bytes_written.clone(), &mount_points) {
            Ok(_) => {
                *status.lock().unwrap() = "Starting verification...".to_string();
                
                // Verification phase
                match verify_integrity(&image_pb, &device_path, verify_progress.clone(), status.clone()) {
                    Ok(_) => {
                        *status.lock().unwrap() = "All operations completed successfully!".to_string();
                        *progress.lock().unwrap() = 1.0;
                        *verify_progress.lock().unwrap() = 1.0;
                    }
                    Err(e) => {
                        let err_msg = format!("Verification Error: {}", e);
                        *status.lock().unwrap() = err_msg.clone();
                        *error.lock().unwrap() = Some(err_msg);
                    }
                }
            }
            Err(e) => {
                let err_msg = format!("Flash Error: {}", e);
                *status.lock().unwrap() = err_msg.clone();
                *error.lock().unwrap() = Some(err_msg);
            }
        }
        
        *is_running.lock().unwrap() = false;
    });
    
    Box::into_raw(Box::new(operation))
}

/// Get the current progress of a flash operation (0.0 to 1.0)
#[no_mangle]
pub extern "C" fn flux_get_progress(operation: *const CFlashOperation) -> c_float {
    if operation.is_null() {
        return 0.0;
    }
    
    unsafe {
        *(*operation).progress.lock().unwrap()
    }
}

/// Get the current verification progress (0.0 to 1.0)
#[no_mangle]
pub extern "C" fn flux_get_verify_progress(operation: *const CFlashOperation) -> c_float {
    if operation.is_null() {
        return 0.0;
    }
    
    unsafe {
        *(*operation).verify_progress.lock().unwrap()
    }
}

/// Get the current status message (caller must free with flux_free_string)
#[no_mangle]
pub extern "C" fn flux_get_status(operation: *const CFlashOperation) -> *mut c_char {
    if operation.is_null() {
        return ptr::null_mut();
    }
    
    unsafe {
        let status = (*operation).status.lock().unwrap().clone();
        CString::new(status).unwrap().into_raw()
    }
}

/// Get bytes written so far
#[no_mangle]
pub extern "C" fn flux_get_bytes_written(operation: *const CFlashOperation) -> u64 {
    if operation.is_null() {
        return 0;
    }
    
    unsafe {
        *(*operation).bytes_written.lock().unwrap()
    }
}

/// Check if operation is still running
#[no_mangle]
pub extern "C" fn flux_is_running(operation: *const CFlashOperation) -> bool {
    if operation.is_null() {
        return false;
    }
    
    unsafe {
        *(*operation).is_running.lock().unwrap()
    }
}

/// Check if operation has an error (returns error message or null)
#[no_mangle]
pub extern "C" fn flux_get_error(operation: *const CFlashOperation) -> *mut c_char {
    if operation.is_null() {
        return ptr::null_mut();
    }
    
    unsafe {
        if let Some(ref err) = *(*operation).error.lock().unwrap() {
            CString::new(err.clone()).unwrap().into_raw()
        } else {
            ptr::null_mut()
        }
    }
}

/// Free a flash operation handle
#[no_mangle]
pub extern "C" fn flux_free_operation(operation: *mut CFlashOperation) {
    if !operation.is_null() {
        unsafe {
            let _ = Box::from_raw(operation);
        }
    }
}

/// Free a string returned by the library
#[no_mangle]
pub extern "C" fn flux_free_string(s: *mut c_char) {
    if !s.is_null() {
        unsafe {
            let _ = CString::from_raw(s);
        }
    }
}

/// Format a byte count into a human-readable string (caller must free)
#[no_mangle]
pub extern "C" fn flux_format_size(bytes: u64) -> *mut c_char {
    let formatted = format_size(bytes);
    CString::new(formatted).unwrap().into_raw()
}

/// Format a duration in seconds into a human-readable string (caller must free)
#[no_mangle]
pub extern "C" fn flux_format_duration(secs: u64) -> *mut c_char {
    let formatted = format_duration(secs);
    CString::new(formatted).unwrap().into_raw()
}
