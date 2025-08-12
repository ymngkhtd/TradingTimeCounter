# Trading Time Counter

A cross-platform desktop application that displays a 5-minute countdown timer overlay for trading sessions.

## Features

- 5-minute countdown timer with overlay display
- Always-on-top window that stays above all applications
- Draggable positioning
- Windows support (cross-platform architecture ready)

## Requirements

- C++17 compiler
- CMake 3.16+
- Windows SDK (for Windows builds)

## Quick Start

### Windows

```bash
# Build using provided script
.\build.ps1

# Or manually
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### Run

```bash
# From build directory
.\Release\tradingTimeCounter.exe

# Or from root directory after build
.\build\Release\tradingTimeCounter.exe
```

## Project Structure

```
TradingTimeCounter/
├── tradingTimeCounter/          # Source code
│   ├── src/                     # Implementation files
│   ├── include/                 # Header files
│   └── CMakeLists.txt          # Build configuration
├── build.ps1                   # Windows build script
├── build.sh                    # Unix build script
└── CMakeLists.txt              # Root build configuration
```

## Development

The application uses a modular architecture:
- **TimerCore**: Static library with core functionality
- **tradingTimeCounter**: Main executable

Built artifacts:
- `TimerCore.lib` - Core timer library
- `tradingTimeCounter.exe` - Main application