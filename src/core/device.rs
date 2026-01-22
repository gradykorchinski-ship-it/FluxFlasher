use anyhow::{Context, Result};
use serde::Deserialize;
use std::process::Command;
use super::utils::format_size;

#[derive(Clone, Debug)]
pub struct UsbDevice {
    pub path: String,
    pub size: String,
    pub size_bytes: u64,
    pub removable: bool,
    pub mounted: bool,
    pub is_system_disk: bool,
    pub mount_points: Vec<String>,
}

#[derive(Debug, Deserialize)]
struct LsblkOutput {
    blockdevices: Vec<LsblkDevice>,
}

#[derive(Debug, Deserialize)]
struct LsblkDevice {
    name: String,
    size: u64,
    #[serde(default)]
    rm: bool,
    #[serde(default)]
    ro: bool,
    #[serde(rename = "type")]
    dev_type: String,
    mountpoint: Option<String>,
    #[serde(default)]
    children: Vec<LsblkDevice>,
}

/// List all removable USB devices on the system
pub fn list_usb_devices() -> Result<Vec<UsbDevice>> {
    let output = Command::new("lsblk")
        .arg("-J")
        .arg("-b")
        .arg("-o")
        .arg("NAME,SIZE,RO,TYPE,MOUNTPOINT,RM")
        .output()
        .context("Failed to run lsblk")?;

    let lsblk: LsblkOutput = serde_json::from_slice(&output.stdout)
        .context("Failed to parse lsblk JSON")?;

    let mut devices = Vec::new();

    for device in lsblk.blockdevices {
        let mut mount_points = Vec::new();
        let is_system = check_system_recursive(&device, &mut mount_points);
        
        if device.dev_type == "disk" && device.rm {
            let size_display = format_size(device.size);
            
            devices.push(UsbDevice {
                path: format!("/dev/{}", device.name),
                size: size_display,
                size_bytes: device.size,
                removable: device.rm,
                mounted: !mount_points.is_empty(),
                is_system_disk: is_system,
                mount_points,
            });
        }
    }
    
    Ok(devices)
}

/// Recursively check if a device or its children contain system mount points
fn check_system_recursive(device: &LsblkDevice, all_mounts: &mut Vec<String>) -> bool {
    let mut is_system = false;
    
    if let Some(mp) = &device.mountpoint {
        all_mounts.push(mp.clone());
        if mp == "/" || mp == "/boot" || mp == "/boot/efi" || mp == "/home" {
            is_system = true;
        }
    }
    
    for child in &device.children {
        if check_system_recursive(child, all_mounts) {
            is_system = true;
        }
    }
    
    is_system
}
