#include "tradingTimeCounter/CountdownTimer.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>

namespace TradingTimeCounter {

CountdownTimer::CountdownTimer(int durationMinutes) 
    : m_totalDuration(durationMinutes * 60)
    , m_remainingSeconds(calculateInitialRemainingTime())
    , m_isRunning(false)
    , m_shouldStop(false)
    , m_callback(nullptr)
    , m_timerThread(nullptr) {
}CountdownTimer::~CountdownTimer() {
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
    
    // Reset remaining time to next 5-minute boundary
    m_remainingSeconds.store(calculateInitialRemainingTime());
    
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
    
    while (!m_shouldStop.load()) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - lastUpdateTime).count();
        
        // Update every 1000ms (1 second)
        if (elapsed >= 1000) {
            int remaining = m_remainingSeconds.load() - 1;
            m_remainingSeconds.store(remaining);
            lastUpdateTime = currentTime;
            
            if (m_callback) {
                m_callback->onTimerUpdate(remaining);
            }

            // Check if timer completed
            if (remaining <= 0) {
                // Timer has completed, stop running
                m_isRunning.store(false);
                
                // Notify completion
                if (m_callback) {
                    m_callback->onTimerCompleted();
                }
                
                // Break out of the loop since the timer is now stopped
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

int CountdownTimer::calculateInitialRemainingTime() const {
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    // Get current minute and second
    int currentMinute = tm.tm_min;
    int currentSecond = tm.tm_sec;
    
    // Calculate how many minutes until the next 5-minute boundary
    int minutesToNext5MinBoundary = (5 - (currentMinute % 5)) % 5;
    
    // If we're exactly on a 5-minute boundary (and it's 0 seconds), start a full 5-minute countdown
    if (minutesToNext5MinBoundary == 0 && currentSecond == 0) {
        return m_totalDuration; // Full 5 minutes
    }
    
    // Calculate total seconds to next boundary
    int secondsToNext5MinBoundary = minutesToNext5MinBoundary * 60 - currentSecond;
    
    // If the result is 0, we're exactly on a boundary, so return full duration
    if (secondsToNext5MinBoundary == 0) {
        return m_totalDuration;
    }
    
    return secondsToNext5MinBoundary;
}

} // namespace TradingTimeCounter
