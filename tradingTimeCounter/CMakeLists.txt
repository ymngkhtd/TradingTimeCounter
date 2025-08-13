cmake_minimum_required(VERSION 3.16)

project(TradingTimeCounter VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent module for downloading dependencies
include(FetchContent)

# Fetch yaml-cpp
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG master  # Use latest stable master branch
)

# Set yaml-cpp options before fetching
set(YAML_CPP_BUILD_TESTS OFF CACHE INTERNAL "")
set(YAML_CPP_BUILD_TOOLS OFF CACHE INTERNAL "")
set(YAML_CPP_BUILD_CONTRIB OFF CACHE INTERNAL "")
set(YAML_CPP_INSTALL OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(yaml-cpp)

# Include directories
include_directories(include)

# Core source files
set(CORE_SOURCES
    src/CountdownTimer.cpp
    src/App.cpp
    src/ConfigManager.cpp
)

# Platform-specific source files
set(PLATFORM_SOURCES)
if(WIN32)
    list(APPEND PLATFORM_SOURCES src/WindowsOverlay.cpp)
elseif(APPLE)
    list(APPEND PLATFORM_SOURCES 
        src/MacOSOverlay.mm
        src/MacOSEventProcessor.mm
    )
endif()

# All source files
set(ALL_SOURCES ${CORE_SOURCES} ${PLATFORM_SOURCES})

# Header files
set(ALL_HEADERS
    include/tradingTimeCounter/ITimerCallback.h
    include/tradingTimeCounter/CountdownTimer.h
    include/tradingTimeCounter/IDisplayManager.h
    include/tradingTimeCounter/App.h
    include/tradingTimeCounter/ConfigManager.h
)

# Platform-specific headers
if(WIN32)
    list(APPEND ALL_HEADERS include/tradingTimeCounter/WindowsOverlay.h)
elseif(APPLE)
    list(APPEND ALL_HEADERS 
        include/tradingTimeCounter/MacOSOverlay.h
        include/tradingTimeCounter/MacOSEventProcessor.h
    )
endif()

# Create timer library
add_library(TimerCore STATIC ${ALL_SOURCES} ${ALL_HEADERS})

# Set target properties
target_include_directories(TimerCore PUBLIC include)
target_compile_features(TimerCore PUBLIC cxx_std_17)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(TimerCore PRIVATE WIN32_LEAN_AND_MEAN)
    target_link_libraries(TimerCore PRIVATE user32 gdi32)
elseif(APPLE)
    # Enable Objective-C++ compilation for .mm files
    set_target_properties(TimerCore PROPERTIES
        COMPILE_FLAGS "-x objective-c++ -fobjc-arc"
    )
    
    # Link with required macOS frameworks
    target_link_libraries(TimerCore PRIVATE
        "-framework Cocoa"
        "-framework Foundation"
        "-framework AppKit"
    )
endif()

# Main executable (placeholder for now)
add_executable(tradingTimeCounter src/main.cpp)
target_link_libraries(tradingTimeCounter TimerCore yaml-cpp)

# Enable threading support
find_package(Threads REQUIRED)
target_link_libraries(TimerCore PRIVATE Threads::Threads yaml-cpp)
