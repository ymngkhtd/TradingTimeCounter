#ifdef _WIN32

#include "tradingTimeCounter/WindowsOverlay.h"
#include <iostream>
#include <stdexcept>

namespace TradingTimeCounter {

// Static member definitions
const wchar_t* WindowsOverlay::WINDOW_CLASS_NAME = L"TradingTimeCounterOverlay";
bool WindowsOverlay::s_classRegistered = false;

WindowsOverlay::WindowsOverlay()
    : m_hwnd(nullptr)
    , m_hdc(nullptr)
    , m_memDC(nullptr)
    , m_bitmap(nullptr)
    , m_oldBitmap(nullptr)
    , m_font(nullptr)
    , m_oldFont(nullptr)
    , m_isVisible(false)
    , m_isDragging(false)
    , m_dragStartPoint{0, 0}
    , m_windowStartPoint{0, 0}
    , m_closeCallback(nullptr)
    , m_positionCallback(nullptr) {
}

WindowsOverlay::~WindowsOverlay() {
    destroy();
}

bool WindowsOverlay::initialize(const DisplayConfig& config) {
    m_config = config;
    
    std::cout << "WindowsOverlay: Starting initialization..." << std::endl;
    
    // Register window class if not already done
    if (!registerWindowClass()) {
        std::cerr << "WindowsOverlay: Failed to register window class" << std::endl;
        return false;
    }
    
    std::cout << "WindowsOverlay: Window class registered successfully" << std::endl;
    
    // Create the window
    if (!createWindow()) {
        std::cerr << "WindowsOverlay: Failed to create window" << std::endl;
        return false;
    }
    
    std::cout << "WindowsOverlay: Window created successfully" << std::endl;
    
    // Create font
    updateFont();
    
    return true;
}

bool WindowsOverlay::createWindow() {
    m_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,              // Remove WS_EX_LAYERED for now
        WINDOW_CLASS_NAME,                              // Class name
        L"Trading Time Counter",                        // Window title
        WS_POPUP,                                      // Window styles
        m_config.positionX,                            // X position
        m_config.positionY,                            // Y position
        m_config.windowWidth,                          // Width
        m_config.windowHeight,                         // Height
        nullptr,                                       // Parent window
        nullptr,                                       // Menu
        GetModuleHandle(nullptr),                      // Instance handle
        this                                           // Additional data
    );
    
    if (!m_hwnd) {
        DWORD error = GetLastError();
        std::cerr << "WindowsOverlay: CreateWindowExW failed with error: " << error << std::endl;
        return false;
    }
    
    std::cout << "WindowsOverlay: Window created successfully, HWND: " << m_hwnd << std::endl;
    
    // Set transparency using SetLayeredWindowAttributes instead
    SetWindowLongPtr(m_hwnd, GWL_EXSTYLE, 
                    GetWindowLongPtr(m_hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(m_hwnd, 0, m_config.opacity, LWA_ALPHA);
    
    return true;
}

void WindowsOverlay::show() {
    if (m_hwnd) {
        std::cout << "WindowsOverlay: Showing window..." << std::endl;
        ShowWindow(m_hwnd, SW_SHOW);
        m_isVisible = true;
        std::cout << "WindowsOverlay: Window shown successfully" << std::endl;
    } else {
        std::cerr << "WindowsOverlay: Cannot show window - hwnd is null" << std::endl;
    }
}

void WindowsOverlay::hide() {
    if (m_hwnd) {
        ShowWindow(m_hwnd, SW_HIDE);
        m_isVisible = false;
    }
}

void WindowsOverlay::updateText(const std::string& text) {
    m_currentText = text;
    
    if (!m_hwnd) {
        std::cerr << "WindowsOverlay: Cannot update text - missing hwnd" << std::endl;
        return;
    }
    
    // Trigger a repaint
    InvalidateRect(m_hwnd, nullptr, TRUE);
    UpdateWindow(m_hwnd);
}

void WindowsOverlay::updateFont() {
    // Delete old font if exists
    if (m_font) {
        DeleteObject(m_font);
        m_font = nullptr;
    }
    
    // Create new font
    int fontWeight = m_config.isBold ? FW_BOLD : FW_NORMAL;
    std::wstring wFontFamily(m_config.fontFamily.begin(), m_config.fontFamily.end());
    
    m_font = CreateFontW(
        m_config.fontSize,          // Height
        0,                          // Width (0 = default)
        0,                          // Escapement
        0,                          // Orientation
        fontWeight,                 // Weight
        FALSE,                      // Italic
        FALSE,                      // Underline
        FALSE,                      // StrikeOut
        DEFAULT_CHARSET,            // CharSet
        OUT_DEFAULT_PRECIS,         // OutPrecision
        CLIP_DEFAULT_PRECIS,        // ClipPrecision
        CLEARTYPE_QUALITY,          // Quality
        DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
        wFontFamily.c_str()         // FaceName
    );
    
    if (!m_font) {
        std::cerr << "WindowsOverlay: Failed to create font" << std::endl;
    } else {
        std::cout << "WindowsOverlay: Font created successfully" << std::endl;
    }
}

SIZE WindowsOverlay::calculateTextSize(const std::string& text) {
    SIZE size = {0, 0};
    
    if (!m_hwnd || text.empty()) {
        return size;
    }
    
    HDC hdc = GetDC(m_hwnd);
    if (hdc) {
        if (m_font) {
            SelectObject(hdc, m_font);
        }
        
        std::wstring wtext(text.begin(), text.end());
        GetTextExtentPoint32W(hdc, wtext.c_str(), (int)wtext.length(), &size);
        
        ReleaseDC(m_hwnd, hdc);
    }
    
    return size;
}

void WindowsOverlay::updateConfig(const DisplayConfig& config) {
    bool needFontUpdate = (config.fontFamily != m_config.fontFamily ||
                          config.fontSize != m_config.fontSize ||
                          config.isBold != m_config.isBold);
    
    bool needResize = (config.windowWidth != m_config.windowWidth ||
                      config.windowHeight != m_config.windowHeight);
    
    m_config = config;
    
    if (m_hwnd) {
        // Update window position and size if needed
        if (needResize) {
            SetWindowPos(m_hwnd, HWND_TOPMOST, 
                        m_config.positionX, m_config.positionY,
                        m_config.windowWidth, m_config.windowHeight,
                        SWP_SHOWWINDOW);
            
            // Recreate bitmap for new size
            if (m_bitmap) {
                SelectObject(m_memDC, m_oldBitmap);
                DeleteObject(m_bitmap);
                m_bitmap = CreateCompatibleBitmap(m_hdc, m_config.windowWidth, m_config.windowHeight);
                m_oldBitmap = (HBITMAP)SelectObject(m_memDC, m_bitmap);
            }
        }
        
        // Update transparency
        SetLayeredWindowAttributes(m_hwnd, 0, m_config.opacity, LWA_ALPHA);
        
        // Update font if needed
        if (needFontUpdate) {
            updateFont();
        }
        
        // Redraw with current text
        updateText(m_currentText);
    }
}

void WindowsOverlay::setPositionLocked(bool locked) {
    m_config.isLocked = locked;
}

void WindowsOverlay::getPosition(int& x, int& y) const {
    if (m_hwnd) {
        RECT rect;
        GetWindowRect(m_hwnd, &rect);
        x = rect.left;
        y = rect.top;
    } else {
        x = m_config.positionX;
        y = m_config.positionY;
    }
}

void WindowsOverlay::setPosition(int x, int y) {
    m_config.positionX = x;
    m_config.positionY = y;
    
    if (m_hwnd) {
        SetWindowPos(m_hwnd, HWND_TOPMOST, x, y, 0, 0, 
                    SWP_NOSIZE | SWP_SHOWWINDOW);
    }
}

bool WindowsOverlay::isVisible() const {
    return m_isVisible && m_hwnd && IsWindowVisible(m_hwnd);
}

void WindowsOverlay::destroy() {
    // Clean up font
    if (m_font) {
        DeleteObject(m_font);
        m_font = nullptr;
    }
    
    // Destroy window
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    
    m_isVisible = false;
}

void WindowsOverlay::setCloseCallback(std::function<void()> callback) {
    m_closeCallback = callback;
}

void WindowsOverlay::setPositionChangeCallback(std::function<void(int, int)> callback) {
    m_positionCallback = callback;
}

bool WindowsOverlay::registerWindowClass() {
    if (s_classRegistered) {
        return true;
    }
    
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    
    if (RegisterClassExW(&wc)) {
        s_classRegistered = true;
        return true;
    }
    
    return false;
}

LRESULT CALLBACK WindowsOverlay::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    WindowsOverlay* pThis = nullptr;
    
    if (uMsg == WM_CREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<WindowsOverlay*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    } else {
        pThis = reinterpret_cast<WindowsOverlay*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    
    if (pThis) {
        return pThis->handleMessage(uMsg, wParam, lParam);
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT WindowsOverlay::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(m_hwnd, &ps);
                
                // Set background color
                HBRUSH bgBrush = CreateSolidBrush(RGB(m_config.backgroundColor.r, 
                                                     m_config.backgroundColor.g, 
                                                     m_config.backgroundColor.b));
                FillRect(hdc, &ps.rcPaint, bgBrush);
                DeleteObject(bgBrush);
                
                // Set text properties
                SetTextColor(hdc, RGB(m_config.textColor.r, 
                                     m_config.textColor.g, 
                                     m_config.textColor.b));
                SetBkMode(hdc, TRANSPARENT);
                
                // Select font
                if (m_font) {
                    SelectObject(hdc, m_font);
                }
                
                // Calculate text position (center)
                std::wstring wtext(m_currentText.begin(), m_currentText.end());
                SIZE textSize;
                GetTextExtentPoint32W(hdc, wtext.c_str(), (int)wtext.length(), &textSize);
                
                int x = (m_config.windowWidth - textSize.cx) / 2;
                int y = (m_config.windowHeight - textSize.cy) / 2;
                
                // Draw text
                TextOutW(hdc, x, y, wtext.c_str(), (int)wtext.length());
                
                EndPaint(m_hwnd, &ps);
            }
            return 0;
            
        case WM_CLOSE:
            if (m_closeCallback) {
                m_closeCallback();
            }
            return 0;
            
        case WM_LBUTTONDOWN:
            if (m_config.isDraggable && !m_config.isLocked) {
                m_isDragging = true;
                GetCursorPos(&m_dragStartPoint);
                int tempX, tempY;
                getPosition(tempX, tempY);
                m_windowStartPoint.x = tempX;
                m_windowStartPoint.y = tempY;
                SetCapture(m_hwnd);
            }
            return 0;
            
        case WM_LBUTTONUP:
            if (m_isDragging) {
                m_isDragging = false;
                ReleaseCapture();
            }
            return 0;
            
        case WM_MOUSEMOVE:
            if (m_isDragging) {
                POINT currentPoint;
                GetCursorPos(&currentPoint);
                
                int deltaX = currentPoint.x - m_dragStartPoint.x;
                int deltaY = currentPoint.y - m_dragStartPoint.y;
                
                int newX = m_windowStartPoint.x + deltaX;
                int newY = m_windowStartPoint.y + deltaY;
                
                setPosition(newX, newY);
                
                if (m_positionCallback) {
                    m_positionCallback(newX, newY);
                }
            }
            return 0;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

} // namespace TradingTimeCounter

#endif // _WIN32
