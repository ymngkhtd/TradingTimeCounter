#!/bin/bash
# Trading Time Counter - Unix Build Script
# For macOS and Linux development

set -e

PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
INSTALL_DIR="$PROJECT_ROOT/install"

BUILD_TYPE="${1:-Release}"
CLEAN="${2:-false}"

echo "=== Trading Time Counter Build Script ==="
echo "Project Root: $PROJECT_ROOT"
echo "Build Type: $BUILD_TYPE"
echo "Clean Build: $CLEAN"

# Clean build directory if requested
if [ "$CLEAN" = "clean" ] || [ "$CLEAN" = "true" ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    rm -rf "$INSTALL_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"

# Configure with CMake
echo "Configuring project with CMake..."
cd "$BUILD_DIR"
cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build the project
echo "Building project..."
cmake --build . --config "$BUILD_TYPE" --parallel $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo "Build completed successfully!"
echo "Executable location: $BUILD_DIR/bin/$BUILD_TYPE/tradingTimeCounter"

# Show generated files
if [ -d "$BUILD_DIR/bin/$BUILD_TYPE" ]; then
    echo ""
    echo "Generated files:"
    ls -la "$BUILD_DIR/bin/$BUILD_TYPE"
fi
