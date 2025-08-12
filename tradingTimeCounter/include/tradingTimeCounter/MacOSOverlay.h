#pragma once

#ifdef __APPLE__

#include "IDisplayManager.h"
#include <memory>
#include <string>

#ifdef __OBJC__
@class NSWindow;
@class NSTextField;
@class MacOSOverlayWindowDelegate;
#else
typedef struct objc_object NSWindow;
typedef struct objc_object NSTextField;
typedef struct objc_object MacOSOverlayWindowDelegate;
#endif

namespace TradingTimeCounter {

/**
 * @brief macOS-specific implementation of IDisplayManager
 * 
 * This class creates a floating window that stays on top of all other windows.
 * It supports transparency, custom fonts, colors, and mouse dragging using Cocoa.
 */
class MacOSOverlay : public IDisplayManager {
public:
    /**
     * @brief Constructor
     */
    MacOSOverlay();
    
    /**
     * @brief Destructor
     */
    ~MacOSOverlay() override;
    
    // Disable copy constructor and assignment operator
    MacOSOverlay(const MacOSOverlay&) = delete;
    MacOSOverlay& operator=(const MacOSOverlay&) = delete;
    
    // IDisplayManager interface implementation
    bool initialize(const DisplayConfig& config) override;
    void show() override;
    void hide() override;
    void updateText(const std::string& text) override;
    void updateConfig(const DisplayConfig& config) override;
    void setPositionLocked(bool locked) override;
    void getPosition(int& x, int& y) const override;
    void setPosition(int x, int y) override;
    bool isVisible() const override;
    void destroy() override;
    void setCloseCallback(std::function<void()> callback) override;
    void setPositionChangeCallback(std::function<void(int, int)> callback) override;

    /**
     * @brief Called by window delegate when window is closing
     */
    void onWindowWillClose();
    
    /**
     * @brief Called by window delegate when window position changes
     */
    void onWindowDidMove();
    
    /**
     * @brief Check if window is draggable
     * @return true if draggable and not locked
     */
    bool isDraggable() const;

private:
    /**
     * @brief Create the overlay window
     * @return true if successful, false otherwise
     */
    bool createWindow();
    
    /**
     * @brief Update font based on current config
     */
    void updateFont();
    
    /**
     * @brief Update window appearance based on current config
     */
    void updateAppearance();
    
    /**
     * @brief Convert RGB color to NSColor
     */
    void* createNSColor(const DisplayConfig::Color& color);

private:
    // Cocoa objects (as void* to avoid Objective-C in header)
    NSWindow* m_window;                             ///< Main window
    NSTextField* m_textField;                       ///< Text display field
    MacOSOverlayWindowDelegate* m_windowDelegate;   ///< Window delegate
    
    // Configuration and state
    DisplayConfig m_config;                         ///< Current display configuration
    std::string m_currentText;                      ///< Currently displayed text
    bool m_isVisible;                               ///< Visibility state
    bool m_isInitialized;                           ///< Initialization state
    
    // Callbacks
    std::function<void()> m_closeCallback;          ///< Close request callback
    std::function<void(int, int)> m_positionCallback; ///< Position change callback
};

} // namespace TradingTimeCounter

#endif // __APPLE__
