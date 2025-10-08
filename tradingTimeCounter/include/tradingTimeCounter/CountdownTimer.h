#pragma once

#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include "ITimerCallback.h"

namespace TradingTimeCounter {

/**
 * @brief High-precision countdown timer with callback support
 * 
 * This class provides a thread-safe countdown timer that can be started,
 * stopped, and reset. It notifies registered callbacks of state changes.
 */
class CountdownTimer {
public:
    /**
     * @brief Construct countdown timer with specified duration
     * @param durationMinutes Timer duration in minutes (default: 5)
     */
    explicit CountdownTimer(int durationMinutes = 5);
    
    /**
     * @brief Destructor - ensures proper cleanup
     */
    ~CountdownTimer();
    
    // Disable copy constructor and assignment operator
    CountdownTimer(const CountdownTimer&) = delete;
    CountdownTimer& operator=(const CountdownTimer&) = delete;
    
    /**
     * @brief Set timer callback for state notifications
     * @param callback Pointer to callback interface
     */
    void setCallback(std::shared_ptr<ITimerCallback> callback);
    
    /**
     * @brief Start the countdown timer
     */
    void start();
    
    /**
     * @brief Stop the countdown timer
     */
    void stop();
    
    /**
     * @brief Reset timer to initial duration
     */
    void reset();
    
    /**
     * @brief Get remaining time in seconds
     * @return Number of seconds remaining
     */
    int getRemainingSeconds() const;
    
    /**
     * @brief Check if timer is currently running
     * @return true if timer is running, false otherwise
     */
    bool isRunning() const;
    
    /**
     * @brief Get formatted time string (MM:SS)
     * @return Formatted time string
     */
    std::string getFormattedTime() const;

private:
    /**
     * @brief Timer thread function
     */
    void timerThreadFunction();
    
    /**
     * @brief Format seconds to MM:SS string
     * @param seconds Number of seconds
     * @return Formatted string
     */
    std::string formatTime(int seconds) const;
    
    /**
     * @brief Calculate initial remaining time to next 5-minute boundary
     * @return Seconds until next 5-minute interval
     */
    int calculateInitialRemainingTime() const;

private:
    const int m_totalDuration;                           ///< Total timer duration in seconds
    std::atomic<int> m_remainingSeconds;                 ///< Remaining seconds (atomic for thread safety)
    std::atomic<bool> m_isRunning;                       ///< Running state flag
    std::atomic<bool> m_shouldStop;                      ///< Stop request flag
    
    std::shared_ptr<ITimerCallback> m_callback;          ///< Timer callback interface
    std::unique_ptr<std::thread> m_timerThread;          ///< Timer execution thread
};

} // namespace TradingTimeCounter
