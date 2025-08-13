#include <iostream>
#include "tradingTimeCounter/App.h"
#include "tradingTimeCounter/ConfigManager.h"

using namespace TradingTimeCounter;

int main() {
    std::cout << "Trading Time Counter v1.0" << std::endl;
    std::cout << "===========================" << std::endl;
    
    try {
        // Create and initialize ConfigManager
        auto configManager = std::make_shared<ConfigManager>("config.yaml");
        
        // Create application instance with ConfigManager (initialization happens in constructor)
        App app(configManager);
        
        // Start the application
        app.start();
        
        std::cout << std::endl;
        std::cout << "Application Controls:" << std::endl;
        std::cout << "- Drag the timer window to move it" << std::endl;
        std::cout << "- Close the timer window to exit" << std::endl;
        std::cout << "- Timer will automatically count down from next 5-minute boundary" << std::endl;
        std::cout << "- Auto-restart: " << (app.isAutoRestartEnabled() ? "Enabled" : "Disabled") << std::endl;
        std::cout << std::endl;
        
        // Run the message loop
        int exitCode = app.run();
        
        // Save configuration before exit
        if (app.saveConfig()) {
            std::cout << "Configuration saved successfully." << std::endl;
        }
        
        std::cout << "Application exiting with code: " << exitCode << std::endl;
        return exitCode;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred!" << std::endl;
        return -1;
    }
}
