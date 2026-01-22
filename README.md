# FluxFlasher - C++ UI with Rust Core

A USB flash tool with a native C++ UI (Qt6) and high-performance Rust core.

## Architecture

- **Rust Core** (`src/lib.rs`, `src/core/`): Device detection, flashing, verification
- **C++ UI** (`cpp/`): Qt6-based native interface
- **FFI Bridge**: C ABI interface generated with cbindgen

## Building

### Prerequisites

- Rust toolchain (cargo, rustc)
- Qt6 development libraries
- CMake 3.16+
- C++17 compiler

### Build Steps

```bash
# Quick build
chmod +x build.sh
./build.sh

# Or manually:
# 1. Build Rust library
cargo build --release

# 2. Build C++ UI
mkdir -p build && cd build
cmake ..
make
```

## Running

```bash
./build/FluxFlasher
```

## Project Structure

```
FluxFlasher/
├── src/
│   ├── lib.rs              # FFI interface
│   └── core/               # Rust business logic
│       ├── device.rs       # USB device detection
│       ├── flash.rs        # Flash operations
│       ├── verify.rs       # Integrity verification
│       └── utils.rs        # Utility functions
├── cpp/
│   ├── main.cpp            # Application entry
│   ├── mainwindow.{h,cpp}  # Main window
│   ├── core_interface.{h,cpp}  # C++ FFI wrapper
│   ├── widgets/            # Reusable UI components
│   └── dialogs/            # Modal dialogs
├── target/
│   ├── fluxflasher.h       # Generated C header
│   └── release/
│       └── libFluxFlasher.so  # Rust library
├── Cargo.toml              # Rust dependencies
├── CMakeLists.txt          # C++ build config
└── build.sh                # Build script
```

## License

Same as original FluxFlasher project.
