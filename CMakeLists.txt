cmake_minimum_required(VERSION 3.16)
project(FluxFlasher VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Include Rust library headers
include_directories(${CMAKE_SOURCE_DIR}/target)

# Source files
set(SOURCES
    cpp/main.cpp
    cpp/mainwindow.cpp
    cpp/core_interface.cpp
    cpp/widgets/stepcard.cpp
    cpp/widgets/progressview.cpp
    cpp/dialogs/devicedialog.cpp
    cpp/dialogs/settingsdialog.cpp
    cpp/dialogs/confirmdialog.cpp
    cpp/dialogs/messagedialog.cpp
)

set(HEADERS
    cpp/mainwindow.h
    cpp/core_interface.h
    cpp/widgets/stepcard.h
    cpp/widgets/progressview.h
    cpp/dialogs/devicedialog.h
    cpp/dialogs/settingsdialog.h
    cpp/dialogs/confirmdialog.h
    cpp/dialogs/messagedialog.h
)

# Create executable
add_executable(FluxFlasher ${SOURCES} ${HEADERS})

# Link Qt6
target_link_libraries(FluxFlasher Qt6::Core Qt6::Widgets)

# Link Rust library
target_link_libraries(FluxFlasher ${CMAKE_SOURCE_DIR}/target/release/libFluxFlasher.so)

# Set output directory
set_target_properties(FluxFlasher PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
