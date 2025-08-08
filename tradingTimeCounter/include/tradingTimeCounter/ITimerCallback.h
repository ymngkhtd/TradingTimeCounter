#pragma once

namespace TradingTimeCounter {

/**
 * @brief Interface for timer event callbacks
 * 
 * This interface provides callbacks for timer state changes.
 * Implementing classes can respond to timer updates and completion.
 */
class ITimerCallback {
public:
    virtual ~ITimerCallback() = default;
    
    /**
     * @brief Called every second when timer updates
     * @param remainingSeconds Number of seconds remaining
     */
    virtual void onTimerUpdate(int remainingSeconds) = 0;
    
    /**
     * @brief Called when timer reaches zero
     */
    virtual void onTimerCompleted() = 0;
    
    /**
     * @brief Called when timer is started
     */
    virtual void onTimerStarted() = 0;
    
    /**
     * @brief Called when timer is stopped
     */
    virtual void onTimerStopped() = 0;
};

} // namespace TradingTimeCounter
