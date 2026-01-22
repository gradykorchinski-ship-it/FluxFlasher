use anyhow::{Context, Result};
use std::io::{BufRead, BufReader};
use std::path::PathBuf;
use std::process::{Command, Stdio};
use std::sync::{Arc, Mutex};

/// Flash an image to a device with progress tracking
pub fn flash_image(
    image_path: &PathBuf, 
    device_path: &str, 
    progress: Arc<Mutex<f32>>, 
    status: Arc<Mutex<String>>, 
    bytes_written: Arc<Mutex<u64>>,
    mount_points: &[String]
) -> Result<()> {
    // 1. Unmount all partitions
    if !mount_points.is_empty() {
        *status.lock().unwrap() = "Unmounting partitions...".to_string();
        for mp in mount_points {
            let umount_status = Command::new("pkexec")
                .arg("umount")
                .arg(mp)
                .status()
                .context("Failed to execute umount")?;
            
            if !umount_status.success() {
                 return Err(anyhow::anyhow!("Failed to unmount {}", mp));
            }
        }
    }

    // 2. Flash with flock and progress
    *status.lock().unwrap() = "Starting write process...".to_string();
    let image_size = std::fs::metadata(image_path)?.len();
    
    let mut child = Command::new("pkexec")
        .arg("flock")
        .arg(device_path)
        .arg("dd")
        .arg(format!("if={}", image_path.display()))
        .arg(format!("of={}", device_path))
        .arg("bs=4M")
        .arg("conv=fsync")
        .arg("status=progress")
        .stdout(Stdio::null())
        .stderr(Stdio::piped())
        .spawn()
        .context("Failed to start dd")?;

    let stderr = child.stderr.take().ok_or_else(|| anyhow::anyhow!("Failed to capture stderr"))?;
    let reader = BufReader::new(stderr);
    
    for line in reader.lines() {
        if let Ok(line) = line {
            // dd status=progress format: "20971520 bytes (21 MB, 20 MiB) copied, ..."
            if let Some(bytes_str) = line.split_whitespace().next() {
                if let Ok(bytes) = bytes_str.parse::<u64>() {
                     *bytes_written.lock().unwrap() = bytes;
                     *progress.lock().unwrap() = (bytes as f32 / image_size as f32).min(0.99);
                }
            }
        }
    }

    let exit_status = child.wait()?;
    if !exit_status.success() {
        return Err(anyhow::anyhow!("Flash process failed (exit code {:?})", exit_status.code()));
    }

    *progress.lock().unwrap() = 1.0;
    *bytes_written.lock().unwrap() = image_size;
    Ok(())
}
