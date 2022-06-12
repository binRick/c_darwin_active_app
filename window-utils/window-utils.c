#pragma once
#include "parson.h"
#include "window-utils.h"

extern AXError _AXUIElementGetWindow(AXUIElementRef, CGWindowID *out);


static int emptyWindowNameAllowed(char *appName) {
  return(0 == strcmp(appName, "Messages"));
}


CGPoint CGWindowGetPosition(CFDictionaryRef window) {
  CFDictionaryRef bounds = CFDictionaryGetValue(window, kCGWindowBounds);
  int             x      = CFDictionaryGetInt(bounds, CFSTR("X"));
  int             y      = CFDictionaryGetInt(bounds, CFSTR("Y"));

  return(CGPointMake(x, y));
}


CGSize CGWindowGetSize(CFDictionaryRef window) {
  CFDictionaryRef bounds = CFDictionaryGetValue(window, kCGWindowBounds);
  int             width  = CFDictionaryGetInt(bounds, CFSTR("Width"));
  int             height = CFDictionaryGetInt(bounds, CFSTR("Height"));

  return(CGSizeMake(width, height));
}


AXUIElementRef AXWindowFromCGWindow(CFDictionaryRef window) {
  CGWindowID     targetWindowId, actualWindowId;
  CFStringRef    targetWindowName, actualWindowTitle;
  CGPoint        targetPosition, actualPosition;
  CGSize         targetSize, actualSize;
  pid_t          pid;
  AXUIElementRef app, appWindow, foundAppWindow;
  int            i;
  CFArrayRef     appWindowList;

  targetWindowId   = CFDictionaryGetInt(window, kCGWindowNumber);
  targetWindowName = CFDictionaryGetValue(window, kCGWindowName);
  targetPosition   = CGWindowGetPosition(window);
  targetSize       = CGWindowGetSize(window);

  pid = CFDictionaryGetInt(window, kCGWindowOwnerPID);
  app = AXUIElementCreateApplication(pid);
  AXUIElementCopyAttributeValue(
    app, kAXWindowsAttribute, (CFTypeRef *)&appWindowList
    );

  foundAppWindow = NULL;
  for (i = 0; i < CFArrayGetCount(appWindowList); i++) {
    appWindow = CFArrayGetValueAtIndex(appWindowList, i);
    if (_AXUIElementGetWindow) {
      _AXUIElementGetWindow(appWindow, &actualWindowId);
      if (actualWindowId == targetWindowId) {
        foundAppWindow = appWindow;
        break;
      } else {
        continue;
      }
    } else {
      AXUIElementCopyAttributeValue(
        appWindow, kAXTitleAttribute, (CFTypeRef *)&actualWindowTitle
        );
      if (!actualWindowTitle
          || CFStringCompare(targetWindowName, actualWindowTitle, 0) != 0) {
        continue;
      }
      actualPosition = AXWindowGetPosition(appWindow);
      if (!CGPointEqualToPoint(targetPosition, actualPosition)) {
        continue;
      }
      actualSize = AXWindowGetSize(appWindow);
      if (!CGSizeEqualToSize(targetSize, actualSize)) {
        continue;
      }
      foundAppWindow = appWindow;
      break;
    }
  }
  CFRelease(app);

  return(foundAppWindow);
} /* AXWindowFromCGWindow */


void AXWindowGetValue(AXUIElementRef window,
                      CFStringRef    attrName,
                      void           *valuePtr) {
  AXValueRef attrValue;

  AXUIElementCopyAttributeValue(window, attrName, (CFTypeRef *)&attrValue);
  AXValueGetValue(attrValue, AXValueGetType(attrValue), valuePtr);
  CFRelease(attrValue);
}


CGPoint AXWindowGetPosition(AXUIElementRef window) {
  CGPoint position;

  AXWindowGetValue(window, kAXPositionAttribute, &position);
  return(position);
}


void AXWindowSetPosition(AXUIElementRef window, CGPoint position) {
  AXValueRef attrValue = AXValueCreate(kAXValueCGPointType, &position);

  AXUIElementSetAttributeValue(window, kAXPositionAttribute, attrValue);
  CFRelease(attrValue);
}


CGSize AXWindowGetSize(AXUIElementRef window) {
  CGSize size;

  AXWindowGetValue(window, kAXSizeAttribute, &size);
  return(size);
}


void AXWindowSetSize(AXUIElementRef window, CGSize size) {
  AXValueRef attrValue = AXValueCreate(kAXValueCGSizeType, &size);

  AXUIElementSetAttributeValue(window, kAXSizeAttribute, attrValue);
  CFRelease(attrValue);
}


int EnumerateWindows(char *pattern,
                     void ( *callback )(CFDictionaryRef window, void *callback_data),
                     void *callback_data) {
  int             patternLen, subPatternLen, count, i, layer;
  char            *subPattern, *starL, *starR, *appName, *windowName, *title;
  CFArrayRef      windowList;
  CFDictionaryRef window;

  if (pattern && *pattern) {
    patternLen    = strlen(pattern);
    starL         = (*pattern == '*') ? "" : "*";
    starR         = (*pattern + (patternLen - 1) == '*') ? "" : "*";
    subPatternLen = patternLen + strlen(starL) + strlen(starR) + 1;
    subPattern    = (char *)malloc(subPatternLen);
    snprintf(subPattern, subPatternLen, "%s%s%s", starL, pattern, starR);
  } else {
    subPattern = pattern;
  }

  windowList = CGWindowListCopyWindowInfo(
    (kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements),
    kCGNullWindowID
    );
  count = 0;
  for (i = 0; i < CFArrayGetCount(windowList); i++) {
    window = CFArrayGetValueAtIndex(windowList, i);

    layer = CFDictionaryGetInt(window, kCGWindowLayer);
    if (layer > 0) {
      continue;
    }

    appName = windowName = title = NULL;
    appName = CFDictionaryCopyCString(window, kCGWindowOwnerName);
    if (!appName || !*appName) {
      goto skip;
    }
    windowName = CFDictionaryCopyCString(window, kCGWindowName);
    if (!windowName || (!*windowName && !emptyWindowNameAllowed(appName))) {
      goto skip;
    }
    title = windowTitle(appName, windowName);

    if (!pattern || fnmatch(subPattern, title, 0) == 0) {
      if (callback) {
        (*callback)(window, callback_data);
      }
      count++;
    }

skip:
    if (title) {
      free(title);
    }
    if (windowName) {
      free(windowName);
    }
    if (appName) {
      free(appName);
    }
  }
  CFRelease(windowList);
  if (subPattern != pattern) {
    free(subPattern);
  }

  return(count);
} /* EnumerateWindows */


char *windowTitle(char *appName, char *windowName) {
  size_t titleSize;
  char   *title;

  if (!appName || !*appName) {
    title  = (char *)malloc(1);
    *title = '\0';
  } else if (!windowName || !*windowName) {
    titleSize = strlen(appName) + 1;
    title     = (char *)malloc(titleSize);
    strncpy(title, appName, titleSize);
  } else {
    titleSize = strlen(appName) + strlen(" - ") + strlen(windowName) + 1;
    title     = (char *)malloc(titleSize);
    snprintf(title, titleSize, "%s - %s", appName, windowName);
  }

  return(title);
}


void PrintWindow(CFDictionaryRef window, void *ctxPtr) {
  LsWinCtx    *ctx                      = (LsWinCtx *)ctxPtr;
  int         windowId                  = CFDictionaryGetInt(window, kCGWindowNumber);
  char        *appName                  = CFDictionaryCopyCString(window, kCGWindowOwnerName);
  char        *windowName               = CFDictionaryCopyCString(window, kCGWindowName);
  char        *title                    = windowTitle(appName, windowName);
  CGPoint     position                  = CGWindowGetPosition(window);
  CGSize      size                      = CGWindowGetSize(window);
  JSON_Value  *root_value               = json_value_init_object();
  JSON_Object *root_object              = json_value_get_object(root_value);
  char        *serialized_string        = NULL;
  char        *pretty_serialized_string = NULL;

  json_object_set_string(root_object, "appName", appName);
  json_object_set_string(root_object, "windowName", windowName);
  json_object_set_string(root_object, "title", title);
  json_object_set_number(root_object, "windowId", windowId);
  json_object_dotset_number(root_object, "size.height", (int)size.height);
  json_object_dotset_number(root_object, "size.width", (int)size.width);
  json_object_dotset_number(root_object, "position.x", (int)position.x);
  json_object_dotset_number(root_object, "position.y", (int)position.y);
  pretty_serialized_string = json_serialize_to_string_pretty(root_value);
  serialized_string        = json_serialize_to_string(root_value);
  if (ctx->id == -1 || ctx->id == windowId) {
    if (ctx->jsonMode) {
      printf("%s", serialized_string);
    }else if (ctx->longDisplay) {
      printf(
        "%s - %s %d %d %d %d %d\n", title,
        appName,
        (int)windowId,
        (int)position.x, (int)position.y,
        (int)size.width, (int)size.height
        );
    }else {
      printf("%d\n", windowId);
    }
    ctx->numFound++;
  }
  json_free_serialized_string(serialized_string);
  json_value_free(root_value);
  free(title);
  free(windowName);
  free(appName);
} /* PrintWindow */
