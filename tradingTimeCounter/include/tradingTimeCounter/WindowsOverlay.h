#pragma once

#ifdef _WIN32

#include "IDisplayManager.h"
#include <windows.h>
#include <memory>
#include <string>

namespace TradingTimeCounter {

/**
 * @brief Windows-specific implementation of IDisplayManager
 * 
 * This class creates a layered window that stays on top of all other windows.
 * It supports transparency, custom fonts, colors, and mouse dragging.
 */
class WindowsOverlay : public IDisplayManager {
public:
    /**
     * @brief Constructor
     */
    WindowsOverlay();
    
    /**
     * @brief Destructor
     */
    ~WindowsOverlay() override;
    
    // Disable copy constructor and assignment operator
    WindowsOverlay(const WindowsOverlay&) = delete;
    WindowsOverlay& operator=(const WindowsOverlay&) = delete;
    
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

private:
    /**
     * @brief Create the overlay window
     * @return true if successful, false otherwise
     */
    bool createWindow();
    
    /**
     * @brief Create or update font based on current config
     */
    void updateFont();
    
    /**
     * @brief Calculate text size for current font and text
     * @param text Text to measure
     * @return SIZE structure with width and height
     */
    SIZE calculateTextSize(const std::string& text);
    
    /**
     * @brief Window procedure for handling Windows messages
     * @param hwnd Window handle
     * @param uMsg Message type
     * @param wParam Message parameter
     * @param lParam Message parameter
     * @return Message handling result
     */
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    
    /**
     * @brief Instance-specific message handler
     * @param uMsg Message type
     * @param wParam Message parameter
     * @param lParam Message parameter
     * @return Message handling result
     */
    LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    // Window handles and resources
    HWND m_hwnd;                                    ///< Window handle
    HDC m_hdc;                                      ///< Device context
    HDC m_memDC;                                    ///< Memory device context
    HBITMAP m_bitmap;                               ///< Bitmap for drawing
    HBITMAP m_oldBitmap;                            ///< Previous bitmap
    HFONT m_font;                                   ///< Current font
    HFONT m_oldFont;                                ///< Previous font
    
    // Configuration and state
    DisplayConfig m_config;                         ///< Current display configuration
    std::string m_currentText;                      ///< Currently displayed text
    bool m_isVisible;                               ///< Visibility state
    bool m_isDragging;                              ///< Currently being dragged
    POINT m_dragStartPoint;                         ///< Drag start position
    POINT m_windowStartPoint;                       ///< Window position at drag start
    
    // Callbacks
    std::function<void()> m_closeCallback;          ///< Close request callback
    std::function<void(int, int)> m_positionCallback; ///< Position change callback
    
    // Class registration
    static const wchar_t* WINDOW_CLASS_NAME;        ///< Window class name
    static bool s_classRegistered;                  ///< Class registration flag
    
    /**
     * @brief Register window class (done once)
     * @return true if successful or already registered
     */
    static bool registerWindowClass();
};

} // namespace TradingTimeCounter

#endif // _WIN32
