#include <iostream>
#include "tradingTimeCounter/App.h"

using namespace TradingTimeCounter;

int main() {
    std::cout << "Trading Time Counter v1.0" << std::endl;
    std::cout << "===========================" << std::endl;
    
    try {
        // Create application instance
        App app;
        
        // Configure display settings
        DisplayConfig config;
        config.fontFamily = "Consolas";          // Monospace font for numbers
        config.fontSize = 28;                    // Larger font for visibility
        config.isBold = true;
        config.textColor = {0, 255, 0};          // Green text
        config.backgroundColor = {0, 0, 0};      // Black background
        config.windowWidth = 150;
        config.windowHeight = 60;
        config.positionX = 100;
        config.positionY = 50;
        config.isDraggable = true;
        config.isLocked = false;
        config.opacity = 220;                    // Slightly transparent
        
        // Initialize application
        if (!app.initialize(config)) {
            std::cerr << "Failed to initialize application!" << std::endl;
            return -1;
        }
        
        // Start the application
        app.start();
        
        std::cout << std::endl;
        std::cout << "Application Controls:" << std::endl;
        std::cout << "- Drag the timer window to move it" << std::endl;
        std::cout << "- Close the timer window to exit" << std::endl;
        std::cout << "- Timer will automatically count down from 5:00" << std::endl;
        std::cout << std::endl;
        
        // Run the message loop
        int exitCode = app.run();
        
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
