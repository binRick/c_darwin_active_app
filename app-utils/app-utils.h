#pragma once
#include "window-utils.h"
#include "submodules/c_stringfn/include/stringfn.h"
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <assert.h>
#include <fnmatch.h>
#include <libproc.h>
#include <mach/mach_time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/proc_info.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define WARN(msg) { fprintf(stderr, APPLICATION_NAME ": " msg "\n"); }
#define DIE(msg) { fprintf(stderr, APPLICATION_NAME ": " msg "\n"); exit(1); }
#define DIE_USAGE(msg) { fprintf(stderr, APPLICATION_NAME ": " msg "\n" USAGE); exit(1); }
#define DIE_OPT(msg) \
    { fprintf(stderr, APPLICATION_NAME ": " msg " -- %c\n" USAGE, optopt); return 1; }

int CFDictionaryGetInt(CFDictionaryRef dict, const void *key);
char *CFDictionaryCopyCString(CFDictionaryRef dict, const void *key);
bool isAuthorizedForScreenRecording();
bool isAuthorizedForAccessibility();
