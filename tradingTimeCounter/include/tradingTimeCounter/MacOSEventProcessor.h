#pragma once

#ifdef __APPLE__

namespace TradingTimeCounter {

/**
 * @brief Helper class to handle macOS event processing
 */
class MacOSEventProcessor {
public:
    /**
     * @brief Process pending macOS events
     * @return true if events were processed, false if should exit
     */
    static bool processPendingEvents();
    
    /**
     * @brief Initialize the macOS application
     */
    static void initializeApplication();
};

} // namespace TradingTimeCounter

#endif // __APPLE__
