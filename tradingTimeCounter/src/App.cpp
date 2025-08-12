#include "tradingTimeCounter/App.h"
#include <iostream>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include "tradingTimeCounter/WindowsOverlay.h"
#include <windows.h>
#elif __APPLE__
#include "tradingTimeCounter/MacOSOverlay.h"
#include "tradingTimeCounter/MacOSEventProcessor.h"
#endif

namespace TradingTimeCounter {

// Static member definition
const int App::TIMER_DURATION_MINUTES;

App::App()
    : m_timer(nullptr)
    , m_display(nullptr)
    , m_isRunning(false)
    , m_shouldExit(false) {
}

App::~App() {
    shutdown();
}

bool App::initialize(const DisplayConfig& displayConfig) {
    try {
        // Store configuration
        m_displayConfig = displayConfig;
        
        // Create timer component
        m_timer = std::make_unique<CountdownTimer>(TIMER_DURATION_MINUTES);
        if (!m_timer) {
            std::cerr << "Failed to create timer component" << std::endl;
            return false;
        }
        
        // Set timer callback - create shared_ptr from this
        m_timer->setCallback(std::shared_ptr<ITimerCallback>(this, [](ITimerCallback*){}));
        
        // Create display component
        std::cout << "Creating display manager..." << std::endl;
        m_display = createDisplayManager();
        if (!m_display) {
            std::cerr << "Failed to create display manager" << std::endl;
            return false;
        }
        
        // Initialize display
        std::cout << "Initializing display with config..." << std::endl;
        if (!m_display->initialize(m_displayConfig)) {
            std::cerr << "Failed to initialize display" << std::endl;
            return false;
        }
        
        // Set display callbacks
        m_display->setCloseCallback([this]() { onWindowCloseRequested(); });
        m_display->setPositionChangeCallback([this](int x, int y) { onWindowPositionChanged(x, y); });
        
        // Initial display update
        m_display->updateText(m_timer->getFormattedTime());
        
        std::cout << "Application initialized successfully" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception during initialization: " << e.what() << std::endl;
        return false;
    }
}

void App::start() {
    if (!m_timer || !m_display) {
        std::cerr << "Cannot start: application not properly initialized" << std::endl;
        return;
    }
    
#ifdef __APPLE__
    // Initialize macOS application
    MacOSEventProcessor::initializeApplication();
#endif
    
    m_isRunning = true;
    m_shouldExit = false;
    
    // Show display
    m_display->show();
    
    // Start timer
    m_timer->start();
    
    std::cout << "Application started - Timer: " << TIMER_DURATION_MINUTES << " minutes" << std::endl;
}

void App::stop() {
    if (m_timer) {
        m_timer->stop();
    }
    
    if (m_display) {
        m_display->hide();
    }
    
    m_isRunning = false;
    std::cout << "Application stopped" << std::endl;
}

int App::run() {
    if (!m_isRunning) {
        std::cerr << "Cannot run: application not started" << std::endl;
        return -1;
    }
    
#ifdef _WIN32
    // Windows message loop
    MSG msg;
    while (!m_shouldExit) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                m_shouldExit = true;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        // Small sleep to prevent high CPU usage
        Sleep(10);
    }
#elif __APPLE__
    // macOS event loop on main thread
    while (!m_shouldExit) {
        // Process macOS events
        MacOSEventProcessor::processPendingEvents();
        
        // Small sleep to prevent high CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
#else
    // For other platforms, we'll need different message loop implementation
    std::cerr << "Message loop not implemented for this platform" << std::endl;
    return -1;
#endif
    
    return 0;
}

void App::shutdown() {
    stop();
    
    if (m_display) {
        m_display->destroy();
        m_display.reset();
    }
    
    m_timer.reset();
    
    std::cout << "Application shutdown complete" << std::endl;
}

bool App::isRunning() const {
    return m_isRunning;
}

void App::updateDisplayConfig(const DisplayConfig& config) {
    m_displayConfig = config;
    
    if (m_display) {
        m_display->updateConfig(config);
    }
}

void App::togglePositionLock() {
    m_displayConfig.isLocked = !m_displayConfig.isLocked;
    
    if (m_display) {
        m_display->setPositionLocked(m_displayConfig.isLocked);
    }
    
    std::cout << "Position lock " << (m_displayConfig.isLocked ? "enabled" : "disabled") << std::endl;
}

void App::resetTimer() {
    if (m_timer) {
        m_timer->reset();
        std::cout << "Timer reset to " << TIMER_DURATION_MINUTES << " minutes" << std::endl;
    }
}

const DisplayConfig& App::getDisplayConfig() const {
    return m_displayConfig;
}

// ITimerCallback interface implementation
void App::onTimerUpdate(int remainingSeconds) {
    if (m_display) {
        std::string formattedTime = m_timer->getFormattedTime();
        m_display->updateText(formattedTime);
    }
    // Only log significant timer milestones to reduce output
    if (remainingSeconds % 30 == 0 || remainingSeconds <= 10) {
        std::cout << "Timer: " << m_timer->getFormattedTime() << " remaining" << std::endl;
    }
}

void App::onTimerCompleted() {
    std::cout << "Timer completed!" << std::endl;
    
    if (m_display) {
        m_display->updateText("00:00");
    }
    
    // Optional: Show completion notification or perform other actions
    // For now, we keep the display showing 00:00
}

void App::onTimerStarted() {
    std::cout << "Timer started" << std::endl;
}

void App::onTimerStopped() {
    std::cout << "Timer stopped" << std::endl;
}

void App::onWindowCloseRequested() {
    std::cout << "Close requested by user" << std::endl;
    m_shouldExit = true;
}

void App::onWindowPositionChanged(int x, int y) {
    m_displayConfig.positionX = x;
    m_displayConfig.positionY = y;
    // Note: Position change is already handled by the display manager
}

std::unique_ptr<IDisplayManager> App::createDisplayManager() {
#ifdef _WIN32
    return std::make_unique<WindowsOverlay>();
#elif __APPLE__
    return std::make_unique<MacOSOverlay>();
#else
    // For other platforms, create appropriate display manager
    std::cerr << "Display manager not implemented for this platform" << std::endl;
    return nullptr;
#endif
}

} // namespace TradingTimeCounter
