#pragma once

#include <string>
#include <functional>

namespace TradingTimeCounter {

/**
 * @brief Display configuration structure
 */
struct DisplayConfig {
    // Font settings
    std::string fontFamily = "Arial";
    int fontSize = 24;
    bool isBold = true;
    
    // Color settings (RGB format)
    struct Color {
        int r, g, b;
        Color(int red = 255, int green = 255, int blue = 255) : r(red), g(green), b(blue) {}
    };
    Color textColor{255, 255, 255};      // White text
    Color backgroundColor{0, 0, 0};      // Black background
    
    // Window settings
    int windowWidth = 120;
    int windowHeight = 50;
    int positionX = 100;                 // Initial X position
    int positionY = 100;                 // Initial Y position
    bool isDraggable = true;             // Can be moved by mouse
    bool isLocked = false;               // Position locked
    int opacity = 200;                   // 0-255, 200 = ~78% opacity
};

/**
 * @brief Abstract interface for display management
 * 
 * This interface abstracts the platform-specific display implementation,
 * allowing for cross-platform support while maintaining clean separation.
 */
class IDisplayManager {
public:
    virtual ~IDisplayManager() = default;
    
    /**
     * @brief Initialize the display window
     * @param config Display configuration
     * @return true if initialization successful, false otherwise
     */
    virtual bool initialize(const DisplayConfig& config) = 0;
    
    /**
     * @brief Show the display window
     */
    virtual void show() = 0;
    
    /**
     * @brief Hide the display window
     */
    virtual void hide() = 0;
    
    /**
     * @brief Update the displayed text
     * @param text Text to display
     */
    virtual void updateText(const std::string& text) = 0;
    
    /**
     * @brief Update display configuration
     * @param config New configuration
     */
    virtual void updateConfig(const DisplayConfig& config) = 0;
    
    /**
     * @brief Set position lock state
     * @param locked true to lock position, false to allow dragging
     */
    virtual void setPositionLocked(bool locked) = 0;
    
    /**
     * @brief Get current window position
     * @param x Reference to store X coordinate
     * @param y Reference to store Y coordinate
     */
    virtual void getPosition(int& x, int& y) const = 0;
    
    /**
     * @brief Set window position
     * @param x X coordinate
     * @param y Y coordinate
     */
    virtual void setPosition(int x, int y) = 0;
    
    /**
     * @brief Check if display is currently visible
     * @return true if visible, false otherwise
     */
    virtual bool isVisible() const = 0;
    
    /**
     * @brief Cleanup and destroy the display
     */
    virtual void destroy() = 0;
    
    /**
     * @brief Set callback for when user requests to close window
     * @param callback Function to call when close requested
     */
    virtual void setCloseCallback(std::function<void()> callback) = 0;
    
    /**
     * @brief Set callback for when window position changes
     * @param callback Function to call with new position (x, y)
     */
    virtual void setPositionChangeCallback(std::function<void(int, int)> callback) = 0;
};

} // namespace TradingTimeCounter
