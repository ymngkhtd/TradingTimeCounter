#include "tradingTimeCounter/CountdownTimer.h"
#include <sstream>
#include <iomanip>

namespace TradingTimeCounter {

CountdownTimer::CountdownTimer(int durationMinutes)
    : m_totalDuration(durationMinutes * 60)
    , m_remainingSeconds(m_totalDuration)
    , m_isRunning(false)
    , m_shouldStop(false)
    , m_callback(nullptr)
    , m_timerThread(nullptr) {
}

CountdownTimer::~CountdownTimer() {
    stop();
}

void CountdownTimer::setCallback(std::shared_ptr<ITimerCallback> callback) {
    m_callback = callback;
}

void CountdownTimer::start() {
    if (m_isRunning.load()) {
        return; // Already running
    }
    
    m_shouldStop.store(false);
    m_isRunning.store(true);
    
    // Create and start timer thread
    m_timerThread = std::make_unique<std::thread>(&CountdownTimer::timerThreadFunction, this);
    
    // Notify callback
    if (m_callback) {
        m_callback->onTimerStarted();
    }
}

void CountdownTimer::stop() {
    if (!m_isRunning.load()) {
        return; // Not running
    }
    
    m_shouldStop.store(true);
    m_isRunning.store(false);
    
    // Wait for thread to finish
    if (m_timerThread && m_timerThread->joinable()) {
        m_timerThread->join();
        m_timerThread.reset();
    }
    
    // Notify callback
    if (m_callback) {
        m_callback->onTimerStopped();
    }
}

void CountdownTimer::reset() {
    bool wasRunning = m_isRunning.load();
    
    // Stop timer if running
    if (wasRunning) {
        stop();
    }
    
    // Reset remaining time
    m_remainingSeconds.store(m_totalDuration);
    
    // Restart if it was running
    if (wasRunning) {
        start();
    }
}

int CountdownTimer::getRemainingSeconds() const {
    return m_remainingSeconds.load();
}

bool CountdownTimer::isRunning() const {
    return m_isRunning.load();
}

std::string CountdownTimer::getFormattedTime() const {
    return formatTime(getRemainingSeconds());
}

void CountdownTimer::timerThreadFunction() {
    auto lastUpdateTime = std::chrono::steady_clock::now();
    
    while (!m_shouldStop.load() && m_remainingSeconds.load() > 0) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - lastUpdateTime).count();
        
        // Update every 1000ms (1 second)
        if (elapsed >= 1000) {
            int remaining = m_remainingSeconds.load() - 1;
            m_remainingSeconds.store(remaining);
            lastUpdateTime = currentTime;
            
            // Notify callback of update
            if (m_callback) {
                m_callback->onTimerUpdate(remaining);
            }
            
            // Check if timer completed
            if (remaining <= 0) {
                m_isRunning.store(false);
                if (m_callback) {
                    m_callback->onTimerCompleted();
                }
                break;
            }
        }
        
        // Sleep for a short time to avoid busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    m_isRunning.store(false);
}

std::string CountdownTimer::formatTime(int seconds) const {
    int minutes = seconds / 60;
    int remainingSeconds = seconds % 60;
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes 
        << ":" << std::setfill('0') << std::setw(2) << remainingSeconds;
    
    return oss.str();
}

} // namespace TradingTimeCounter
