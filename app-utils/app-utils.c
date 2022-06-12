#pragma once
#include "app-utils.h"
#include <fnmatch.h>

static int emptyWindowNameAllowed(char *appName) {
    return 0 == strcmp(appName, "Messages");
}
extern AXError _AXUIElementGetWindow(AXUIElementRef, CGWindowID *out);

int EnumerateWindows(
    char *pattern,
    void(*callback)(CFDictionaryRef window, void *callback_data),
    void *callback_data
) {
    int patternLen, subPatternLen, count, i, layer;
    char *subPattern, *starL, *starR, *appName, *windowName, *title;
CFArrayRef windowList;
    CFDictionaryRef window;
    if(pattern && *pattern) {
        patternLen = strlen(pattern);
        starL = (*pattern == '*') ? "" : "*";
        starR = (*pattern + (patternLen - 1) == '*') ? "" : "*";
        subPatternLen = patternLen + strlen(starL) + strlen(starR) + 1;
        subPattern = (char *)malloc(subPatternLen);
        snprintf(subPattern, subPatternLen, "%s%s%s", starL, pattern, starR);
    } else {
        subPattern = pattern;
    }

    windowList = CGWindowListCopyWindowInfo(
        (kCGWindowListOptionOnScreenOnly|kCGWindowListExcludeDesktopElements),
        kCGNullWindowID
    );
    count = 0;
    for(i = 0; i < CFArrayGetCount(windowList); i++) {
        window = CFArrayGetValueAtIndex(windowList, i);

        layer = CFDictionaryGetInt(window, kCGWindowLayer);
        if(layer > 0) continue;

        appName = windowName = title = NULL;
        appName = CFDictionaryCopyCString(window, kCGWindowOwnerName);
        if(!appName || !*appName) goto skip;
        windowName = CFDictionaryCopyCString(window, kCGWindowName);
        if(!windowName || (!*windowName && !emptyWindowNameAllowed(appName)))
            goto skip;
        title = windowTitle(appName, windowName);

        if(!pattern || fnmatch(subPattern, title, 0) == 0) {
            if(callback) (*callback)(window, callback_data);
            count++;
        }

      skip:
        if(title) free(title);
        if(windowName) free(windowName);
        if(appName) free(appName);
    }
    CFRelease(windowList);
    if(subPattern != pattern) free(subPattern);

    return count;
}

int CFDictionaryGetInt(CFDictionaryRef dict, const void *key) {
    int isSuccess, value;

    isSuccess = CFNumberGetValue(
        CFDictionaryGetValue(dict, key), kCFNumberIntType, &value
    );

    return isSuccess ? value : 0;
}

char *CFDictionaryCopyCString(CFDictionaryRef dict, const void *key) {
    const void *dictValue;
    CFIndex length;
    int maxSize, isSuccess;
    char *value;

    dictValue = CFDictionaryGetValue(dict, key);
    if(dictValue == NULL) return NULL;
    length = CFStringGetLength(dictValue);
    maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
    if(length == 0 || maxSize == 0) {
        value = (char *)malloc(1);
        *value = '\0';
        return value;
    }

    value = (char *)malloc(maxSize);
    isSuccess = CFStringGetCString(
        dictValue, value, maxSize, kCFStringEncodingUTF8
    );

    return isSuccess ? value : NULL;
}

char *windowTitle(char *appName, char *windowName) {
    size_t titleSize;
    char *title;

    if(!appName || !*appName) {
        title = (char *)malloc(1);
        *title = '\0';
    } else if(!windowName || !*windowName) {
        titleSize = strlen(appName) + 1;
        title = (char *)malloc(titleSize);
        strncpy(title, appName, titleSize);
    } else {
        titleSize = strlen(appName) + strlen(" - ") + strlen(windowName) + 1;
        title = (char *)malloc(titleSize);
        snprintf(title, titleSize, "%s - %s", appName, windowName);
    }

    return title;
}

CGPoint CGWindowGetPosition(CFDictionaryRef window) {
    CFDictionaryRef bounds = CFDictionaryGetValue(window, kCGWindowBounds);
    int x = CFDictionaryGetInt(bounds, CFSTR("X"));
    int y = CFDictionaryGetInt(bounds, CFSTR("Y"));
    return CGPointMake(x, y);
}

CGSize CGWindowGetSize(CFDictionaryRef window) {
    CFDictionaryRef bounds = CFDictionaryGetValue(window, kCGWindowBounds);
    int width = CFDictionaryGetInt(bounds, CFSTR("Width"));
    int height = CFDictionaryGetInt(bounds, CFSTR("Height"));
    return CGSizeMake(width, height);
}

bool isAuthorizedForScreenRecording() {
    if (MAC_OS_X_VERSION_MIN_REQUIRED < 101500) {
        return 1;
    } else {
        CGDisplayStreamFrameAvailableHandler handler =
            ^(CGDisplayStreamFrameStatus status,
              uint64_t display_time,
              IOSurfaceRef frame_surface,
              CGDisplayStreamUpdateRef updateRef) { return; };
        CGDisplayStreamRef stream =
            CGDisplayStreamCreate(CGMainDisplayID(), 1, 1, 'BGRA', NULL, handler);
        if (stream == NULL) {
            return 0;
        } else {
            CFRelease(stream);
            return 1;
        }
    }
}

bool isAuthorizedForAccessibility() {
#if MAC_OS_X_VERSION_MIN_REQUIRED < 1090
    return AXAPIEnabled() || AXIsProcessTrusted();
#else
    return AXIsProcessTrusted();
#endif
}

#pragma GCC diagnostic ignored "-Waddress"
AXUIElementRef AXWindowFromCGWindow(CFDictionaryRef window) {
    CGWindowID targetWindowId, actualWindowId;
    CFStringRef targetWindowName, actualWindowTitle;
    CGPoint targetPosition, actualPosition;
    CGSize targetSize, actualSize;
    pid_t pid;
    AXUIElementRef app, appWindow, foundAppWindow;
    int i;
    CFArrayRef appWindowList;

    targetWindowId = CFDictionaryGetInt(window, kCGWindowNumber);
    targetWindowName = CFDictionaryGetValue(window, kCGWindowName);
    targetPosition = CGWindowGetPosition(window);
    targetSize = CGWindowGetSize(window);

    pid = CFDictionaryGetInt(window, kCGWindowOwnerPID);
    app = AXUIElementCreateApplication(pid);
    AXUIElementCopyAttributeValue(
        app, kAXWindowsAttribute, (CFTypeRef *)&appWindowList
    );

    foundAppWindow = NULL;
    for(i = 0; i < CFArrayGetCount(appWindowList); i++) {
        appWindow = CFArrayGetValueAtIndex(appWindowList, i);
        if(_AXUIElementGetWindow) {
            _AXUIElementGetWindow(appWindow, &actualWindowId);
            if(actualWindowId == targetWindowId) {
                foundAppWindow = appWindow;
                break;
            } else {
                continue;
            }
        } else {
            AXUIElementCopyAttributeValue(
                appWindow, kAXTitleAttribute, (CFTypeRef *)&actualWindowTitle
            );
            if( !actualWindowTitle ||
                CFStringCompare(targetWindowName, actualWindowTitle, 0) != 0 )
            {
                continue;
            }
            actualPosition = AXWindowGetPosition(appWindow);
            if(!CGPointEqualToPoint(targetPosition, actualPosition)) continue;
            actualSize = AXWindowGetSize(appWindow);
            if(!CGSizeEqualToSize(targetSize, actualSize)) continue;
            foundAppWindow = appWindow;
            break;
        }
    }
    CFRelease(app);

    return foundAppWindow;
}

void AXWindowGetValue(
    AXUIElementRef window,
    CFStringRef attrName,
    void *valuePtr
) {
    AXValueRef attrValue;
    AXUIElementCopyAttributeValue(window, attrName, (CFTypeRef *)&attrValue);
    AXValueGetValue(attrValue, AXValueGetType(attrValue), valuePtr);
    CFRelease(attrValue);
}

CGPoint AXWindowGetPosition(AXUIElementRef window) {
    CGPoint position;
    AXWindowGetValue(window, kAXPositionAttribute, &position);
    return position;
}

void AXWindowSetPosition(AXUIElementRef window, CGPoint position) {
    AXValueRef attrValue = AXValueCreate(kAXValueCGPointType, &position);
    AXUIElementSetAttributeValue(window, kAXPositionAttribute, attrValue);
    CFRelease(attrValue);
}

CGSize AXWindowGetSize(AXUIElementRef window) {
    CGSize size;
    AXWindowGetValue(window, kAXSizeAttribute, &size);
    return size;
}

void AXWindowSetSize(AXUIElementRef window, CGSize size) {
    AXValueRef attrValue = AXValueCreate(kAXValueCGSizeType, &size);
    AXUIElementSetAttributeValue(window, kAXSizeAttribute, attrValue);
    CFRelease(attrValue);
}


/* ======================================================================== */
