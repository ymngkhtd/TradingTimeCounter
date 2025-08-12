#ifdef __APPLE__

#include "tradingTimeCounter/MacOSEventProcessor.h"
#import <Cocoa/Cocoa.h>

namespace TradingTimeCounter {

bool MacOSEventProcessor::processPendingEvents() {
    @autoreleasepool {
        NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                            untilDate:[NSDate dateWithTimeIntervalSinceNow:0.01]
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES];
        if (event) {
            [NSApp sendEvent:event];
            return true;
        }
        return false;
    }
}

void MacOSEventProcessor::initializeApplication() {
    @autoreleasepool {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyAccessory];
        [NSApp finishLaunching];
    }
}

} // namespace TradingTimeCounter

#endif // __APPLE__
