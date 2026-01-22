use anyhow::{Context, Result};
use sha2::{Sha256, Digest};
use std::fs::File;
use std::io::{BufReader, Read};
use std::path::PathBuf;
use std::process::{Command, Stdio};
use std::sync::{Arc, Mutex};

/// Verify the integrity of a flashed device by comparing SHA256 hashes
pub fn verify_integrity(
    image_path: &PathBuf,
    device_path: &str,
    progress: Arc<Mutex<f32>>,
    status: Arc<Mutex<String>>
) -> Result<()> {
    *status.lock().unwrap() = "Verifying: Hashing source image...".to_string();
    *progress.lock().unwrap() = 0.0;
    
    let mut file = File::open(image_path)?;
    let total_size = file.metadata()?.len();
    let mut hasher = Sha256::new();
    let mut buffer = [0u8; 1024 * 1024]; // 1MB buffer
    let mut read_so_far = 0;

    loop {
        let n = file.read(&mut buffer)?;
        if n == 0 { break; }
        hasher.update(&buffer[..n]);
        read_so_far += n as u64;
         *progress.lock().unwrap() = (read_so_far as f32 / total_size as f32) * 0.5;
    }
    let expected_hash = hasher.finalize();

    *status.lock().unwrap() = "Verifying: Hashing device content...".to_string();
    
    let mut child = Command::new("pkexec")
        .arg("dd")
        .arg(format!("if={}", device_path))
        .arg(format!("count={}", total_size))
        .arg("iflag=count_bytes")
        .arg("bs=4M")
        .arg("status=none")
        .stdout(Stdio::piped())
        .spawn()?;

    let stdout = child.stdout.take().ok_or_else(|| anyhow::anyhow!("Failed to open device stream"))?;
    let mut reader = BufReader::new(stdout);
    let mut dev_hasher = Sha256::new();
    let mut dev_read_so_far = 0;
    
    loop {
        let n = reader.read(&mut buffer)?;
        if n == 0 { break; }
        dev_hasher.update(&buffer[..n]);
        dev_read_so_far += n as u64;
        // Map 0.5 -> 1.0
        *progress.lock().unwrap() = 0.5 + ((dev_read_so_far as f32 / total_size as f32) * 0.5);
    }
    
    let _ = child.wait();
    
    let actual_hash = dev_hasher.finalize();
    
    if expected_hash != actual_hash {
        return Err(anyhow::anyhow!("Verification failed: Hash mismatch!"));
    }

    *status.lock().unwrap() = "Verification Successful!".to_string();
    *progress.lock().unwrap() = 1.0;
    Ok(())
}
