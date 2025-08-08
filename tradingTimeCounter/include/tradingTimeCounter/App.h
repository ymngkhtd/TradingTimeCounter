#pragma once

#include "ITimerCallback.h"
#include "IDisplayManager.h"
#include "CountdownTimer.h"
#include <memory>
#include <string>

namespace TradingTimeCounter {

/**
 * @brief Main application class that coordinates Timer and Display modules
 * 
 * This class implements the ITimerCallback interface to receive timer events
 * and manages the overall application lifecycle, connecting the timer logic
 * with the display management.
 */
class App : public ITimerCallback {
public:
    /**
     * @brief Constructor
     */
    App();
    
    /**
     * @brief Destructor
     */
    ~App() override;
    
    // Disable copy constructor and assignment operator
    App(const App&) = delete;
    App& operator=(const App&) = delete;
    
    /**
     * @brief Initialize the application
     * @param displayConfig Initial display configuration
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const DisplayConfig& displayConfig = DisplayConfig{});
    
    /**
     * @brief Start the application (show window and start timer)
     */
    void start();
    
    /**
     * @brief Stop the application (hide window and stop timer)
     */
    void stop();
    
    /**
     * @brief Run the application message loop
     * @return Application exit code
     */
    int run();
    
    /**
     * @brief Shutdown and cleanup the application
     */
    void shutdown();
    
    /**
     * @brief Check if application is currently running
     * @return true if running, false otherwise
     */
    bool isRunning() const;
    
    /**
     * @brief Update display configuration
     * @param config New display configuration
     */
    void updateDisplayConfig(const DisplayConfig& config);
    
    /**
     * @brief Toggle position lock state
     */
    void togglePositionLock();
    
    /**
     * @brief Reset timer to initial duration
     */
    void resetTimer();
    
    /**
     * @brief Get current display configuration
     * @return Current display configuration
     */
    const DisplayConfig& getDisplayConfig() const;

    // ITimerCallback interface implementation
    void onTimerUpdate(int remainingSeconds) override;
    void onTimerCompleted() override;
    void onTimerStarted() override;
    void onTimerStopped() override;

private:
    /**
     * @brief Handle window close request
     */
    void onWindowCloseRequested();
    
    /**
     * @brief Handle window position change
     * @param x New X position
     * @param y New Y position
     */
    void onWindowPositionChanged(int x, int y);
    
    /**
     * @brief Create platform-specific display manager
     * @return Pointer to display manager instance
     */
    std::unique_ptr<IDisplayManager> createDisplayManager();

private:
    // Core components
    std::unique_ptr<CountdownTimer> m_timer;           ///< Timer component
    std::unique_ptr<IDisplayManager> m_display;        ///< Display component
    
    // Application state
    bool m_isRunning;                                  ///< Application running state
    bool m_shouldExit;                                 ///< Exit request flag
    DisplayConfig m_displayConfig;                     ///< Current display configuration
    
    // Constants
    static const int TIMER_DURATION_MINUTES = 5;       ///< Fixed timer duration
};

} // namespace TradingTimeCounter
