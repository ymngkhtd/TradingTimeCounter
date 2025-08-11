#ifdef __APPLE__

#include "tradingTimeCounter/MacOSOverlay.h"
#import <Cocoa/Cocoa.h>

// Custom text field with vertical centering
@interface CenteredTextField : NSTextField
@end

@implementation CenteredTextField

- (void)drawRect:(NSRect)dirtyRect {
    NSRect textRect = [self bounds];
    NSAttributedString* attributedString = [self attributedStringValue];
    
    if (attributedString.length > 0) {
        NSSize textSize = [attributedString size];
        
        // Calculate vertical centering
        CGFloat yOffset = (textRect.size.height - textSize.height) / 2.0;
        textRect.origin.y += yOffset;
        textRect.size.height = textSize.height;
        
        // Draw the text centered
        [attributedString drawInRect:textRect];
    } else {
        [super drawRect:dirtyRect];
    }
}

@end

// Custom view to handle window dragging
@interface DraggableView : NSView {
    NSPoint initialLocation;
}
@property (nonatomic, assign) TradingTimeCounter::MacOSOverlay* overlay;
@end

@implementation DraggableView

- (void)mouseDown:(NSEvent *)event {
    if (self.overlay && self.overlay->isDraggable()) {
        initialLocation = [event locationInWindow];
    }
}

- (void)mouseDragged:(NSEvent *)event {
    if (self.overlay && self.overlay->isDraggable()) {
        NSRect screenVisibleFrame = [[NSScreen mainScreen] visibleFrame];
        NSRect windowFrame = [[self window] frame];
        NSPoint currentLocation = [event locationInWindow];
        NSPoint newOrigin = windowFrame.origin;

        // Calculate the new window position
        newOrigin.x += (currentLocation.x - initialLocation.x);
        newOrigin.y += (currentLocation.y - initialLocation.y);

        // Make sure we don't move the window off the screen
        if ((newOrigin.y + windowFrame.size.height) > (screenVisibleFrame.origin.y + screenVisibleFrame.size.height)) {
            newOrigin.y = screenVisibleFrame.origin.y + (screenVisibleFrame.size.height - windowFrame.size.height);
        }

        if (newOrigin.y < screenVisibleFrame.origin.y) {
            newOrigin.y = screenVisibleFrame.origin.y;
        }

        if (newOrigin.x < screenVisibleFrame.origin.x) {
            newOrigin.x = screenVisibleFrame.origin.x;
        }

        if ((newOrigin.x + windowFrame.size.width) > (screenVisibleFrame.origin.x + screenVisibleFrame.size.width)) {
            newOrigin.x = screenVisibleFrame.origin.x + (screenVisibleFrame.size.width - windowFrame.size.width);
        }

        // Set the origin to the new location
        [[self window] setFrameOrigin:newOrigin];
    }
}

@end

// Objective-C window delegate to handle window events
@interface MacOSOverlayWindowDelegate : NSObject <NSWindowDelegate>
@property (nonatomic, assign) TradingTimeCounter::MacOSOverlay* cppOverlay;
@end

@implementation MacOSOverlayWindowDelegate

- (void)windowWillClose:(NSNotification *)notification {
    if (self.cppOverlay) {
        self.cppOverlay->onWindowWillClose();
    }
}

- (void)windowDidMove:(NSNotification *)notification {
    if (self.cppOverlay) {
        self.cppOverlay->onWindowDidMove();
    }
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
    // Allow the window to close
    return YES;
}

@end

namespace TradingTimeCounter {

MacOSOverlay::MacOSOverlay()
    : m_window(nullptr)
    , m_textField(nullptr)
    , m_windowDelegate(nullptr)
    , m_isVisible(false)
    , m_isInitialized(false)
    , m_currentText("00:00")
{
}

MacOSOverlay::~MacOSOverlay() {
    destroy();
}

bool MacOSOverlay::initialize(const DisplayConfig& config) {
    @autoreleasepool {
        m_config = config;
        
        if (!createWindow()) {
            return false;
        }
        
        updateAppearance();
        updateFont();
        updateText(m_currentText);
        
        m_isInitialized = true;
        return true;
    }
}

void MacOSOverlay::show() {
    dispatch_async(dispatch_get_main_queue(), ^{
        @autoreleasepool {
            if (m_window && m_isInitialized) {
                [m_window makeKeyAndOrderFront:nil];
                [m_window setLevel:NSFloatingWindowLevel];
                [m_window orderFrontRegardless];
                m_isVisible = true;
                
                // Force the application to process events to ensure window appears
                [NSApp activateIgnoringOtherApps:YES];
            }
        }
    });
}

void MacOSOverlay::hide() {
    @autoreleasepool {
        if (m_window) {
            [m_window orderOut:nil];
            m_isVisible = false;
        }
    }
}

void MacOSOverlay::updateText(const std::string& text) {
    m_currentText = text;
    
    // Ensure UI updates happen on the main thread
    dispatch_async(dispatch_get_main_queue(), ^{
        @autoreleasepool {
            if (m_textField) {
                NSString* nsText = [NSString stringWithUTF8String:m_currentText.c_str()];
                [m_textField setStringValue:nsText];
                [m_textField setNeedsDisplay:YES];
            }
        }
    });
}

void MacOSOverlay::updateConfig(const DisplayConfig& config) {
    @autoreleasepool {
        m_config = config;
        if (m_isInitialized) {
            updateAppearance();
            updateFont();
            setPosition(config.positionX, config.positionY);
            
            // Update window size
            if (m_window) {
                NSRect frame = [m_window frame];
                frame.size.width = config.windowWidth;
                frame.size.height = config.windowHeight;
                [m_window setFrame:frame display:YES];
            }
        }
    }
}

void MacOSOverlay::setPositionLocked(bool locked) {
    m_config.isLocked = locked;
    @autoreleasepool {
        if (m_window) {
            [m_window setMovable:!locked];
        }
    }
}

void MacOSOverlay::getPosition(int& x, int& y) const {
    @autoreleasepool {
        if (m_window) {
            NSRect frame = [m_window frame];
            x = static_cast<int>(frame.origin.x);
            y = static_cast<int>(frame.origin.y);
        } else {
            x = m_config.positionX;
            y = m_config.positionY;
        }
    }
}

void MacOSOverlay::setPosition(int x, int y) {
    @autoreleasepool {
        m_config.positionX = x;
        m_config.positionY = y;
        
        if (m_window) {
            NSRect frame = [m_window frame];
            frame.origin.x = x;
            frame.origin.y = y;
            [m_window setFrame:frame display:YES];
        }
    }
}

bool MacOSOverlay::isVisible() const {
    return m_isVisible;
}

void MacOSOverlay::destroy() {
    @autoreleasepool {
        if (m_window) {
            [m_window close];
            m_window = nullptr;
        }
        
        // In ARC mode, we don't need to manually release objects
        m_windowDelegate = nullptr;
        m_textField = nullptr;
        m_isInitialized = false;
        m_isVisible = false;
    }
}

void MacOSOverlay::setCloseCallback(std::function<void()> callback) {
    m_closeCallback = callback;
}

void MacOSOverlay::setPositionChangeCallback(std::function<void(int, int)> callback) {
    m_positionCallback = callback;
}

void MacOSOverlay::onWindowWillClose() {
    m_isVisible = false;
    if (m_closeCallback) {
        m_closeCallback();
    }
}

void MacOSOverlay::onWindowDidMove() {
    if (m_positionCallback && m_window) {
        @autoreleasepool {
            NSRect frame = [m_window frame];
            int x = static_cast<int>(frame.origin.x);
            int y = static_cast<int>(frame.origin.y);
            m_config.positionX = x;
            m_config.positionY = y;
            m_positionCallback(x, y);
        }
    }
}

bool MacOSOverlay::isDraggable() const {
    return m_config.isDraggable && !m_config.isLocked;
}

bool MacOSOverlay::createWindow() {
    @autoreleasepool {
        // Create window delegate
        m_windowDelegate = [[MacOSOverlayWindowDelegate alloc] init];
        m_windowDelegate.cppOverlay = this;
        
        // Create window - always use borderless style for clean appearance
        NSRect contentRect = NSMakeRect(m_config.positionX, m_config.positionY, 
                                       m_config.windowWidth, m_config.windowHeight);
        
        // Use borderless window style for clean overlay appearance
        NSWindowStyleMask styleMask = NSWindowStyleMaskBorderless;
        
        m_window = [[NSWindow alloc] initWithContentRect:contentRect
                                               styleMask:styleMask
                                                 backing:NSBackingStoreBuffered
                                                   defer:NO];
        
        if (!m_window) {
            return false;
        }
        
        // Configure window properties
        [m_window setDelegate:m_windowDelegate];
        [m_window setLevel:NSFloatingWindowLevel];
        [m_window setOpaque:NO];
        [m_window setMovable:NO]; // We'll handle dragging manually
        [m_window setRestorable:NO];
        [m_window setAcceptsMouseMovedEvents:YES];
        [m_window setCollectionBehavior:NSWindowCollectionBehaviorCanJoinAllSpaces | 
                                        NSWindowCollectionBehaviorStationary];
        
        // Create custom draggable content view
        NSRect contentViewRect = NSMakeRect(0, 0, m_config.windowWidth, m_config.windowHeight);
        DraggableView* contentView = [[DraggableView alloc] initWithFrame:contentViewRect];
        contentView.overlay = this;
        [m_window setContentView:contentView];
        
        // Create text field with vertical centering
        NSRect textRect = NSMakeRect(0, 0, m_config.windowWidth, m_config.windowHeight);
        m_textField = [[CenteredTextField alloc] initWithFrame:textRect];
        
        if (!m_textField) {
            return false;
        }
        
        // Configure text field
        [m_textField setBezeled:NO];
        [m_textField setDrawsBackground:YES];
        [m_textField setEditable:NO];
        [m_textField setSelectable:NO];
        [m_textField setAlignment:NSTextAlignmentCenter];
        [m_textField setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
        
        // Enable vertical centering by using the cell's alignment
        NSTextFieldCell* cell = [m_textField cell];
        [cell setAlignment:NSTextAlignmentCenter];
        [cell setUsesSingleLineMode:YES];
        [cell setLineBreakMode:NSLineBreakByTruncatingTail];
        
        // Set the text field to use the center baseline
        [m_textField setBaselineAdjustment:NSBaselineAdjustmentAlignCenters];
        
        // Add text field to window
        [[m_window contentView] addSubview:m_textField];
        
        return true;
    }
}

void MacOSOverlay::updateFont() {
    @autoreleasepool {
        if (m_textField) {
            NSString* fontName = [NSString stringWithUTF8String:m_config.fontFamily.c_str()];
            NSFont* font = nil;
            
            if (m_config.isBold) {
                font = [NSFont boldSystemFontOfSize:m_config.fontSize];
            } else {
                font = [NSFont systemFontOfSize:m_config.fontSize];
            }
            
            // Try to use the specified font family
            NSFont* customFont = [NSFont fontWithName:fontName size:m_config.fontSize];
            if (customFont) {
                font = customFont;
            }
            
            [m_textField setFont:font];
        }
    }
}

void MacOSOverlay::updateAppearance() {
    @autoreleasepool {
        if (m_textField) {
            // Set text color
            NSColor* textColor = [NSColor colorWithRed:m_config.textColor.r / 255.0
                                                green:m_config.textColor.g / 255.0
                                                 blue:m_config.textColor.b / 255.0
                                                alpha:1.0];
            [m_textField setTextColor:textColor];
            
            // Set background color with opacity
            NSColor* backgroundColor = [NSColor colorWithRed:m_config.backgroundColor.r / 255.0
                                                      green:m_config.backgroundColor.g / 255.0
                                                       blue:m_config.backgroundColor.b / 255.0
                                                      alpha:m_config.opacity / 255.0];
            [m_textField setBackgroundColor:backgroundColor];
        }
        
        if (m_window) {
            // Set window opacity
            [m_window setAlphaValue:m_config.opacity / 255.0];
        }
    }
}

void* MacOSOverlay::createNSColor(const DisplayConfig::Color& color) {
    @autoreleasepool {
        NSColor* nsColor = [NSColor colorWithRed:color.r / 255.0
                                           green:color.g / 255.0
                                            blue:color.b / 255.0
                                           alpha:1.0];
        return (__bridge void*)nsColor;
    }
}

} // namespace TradingTimeCounter

#endif // __APPLE__
