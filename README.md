# Trading Time Counter

## System Functionality
The Trading Time Counter is a cross-platform desktop application that displays a five-minute countdown timer at the top of the screen. The application is currently implemented for Windows, with plans for cross-platform support in the future.

## Architecture Design
The application follows a high-cohesion, low-coupling design with the following modules:

### Core Modules
- **Timer Module**: Pure logic module for countdown functionality
  - `CountdownTimer`: Core countdown implementation
  - `ITimerCallback`: Callback interface for timer events
- **Display Module**: Abstract display interface for cross-platform support
  - `IDisplayManager`: Abstract display management interface
  - `WindowsOverlay`: Windows-specific top-level window implementation
- **Application Module**: Application lifecycle and coordination
  - `App`: Main application class
  - `main.cpp`: Entry point

### Features
- Fixed 5-minute countdown timer
- Configurable font, color, and size
- Mouse draggable positioning with lock/unlock option
- Always-on-top display
- Cross-platform architecture (Windows first, extensible)

## Dependencies
- C++17 or higher
- CMake for building the project

## Usage Instructions
1. Clone the repository to your local machine.
2. Navigate to the project directory.
3. Create a build directory and navigate into it:
   mkdir build
   cd build
4. Run CMake to configure the project:
   cmake ..
5. Build the application:
   cmake --build .
6. Run the application:
   ./tradingTimeCounter

This will start the countdown timer, which will be displayed at the top of the screen.