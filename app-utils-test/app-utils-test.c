#define APPLICATION_NAME "app-utils-test"
#include "app-utils-test.h"
#include "app-utils.h"
#include "submodules/log.h/log.h"
#include "window-utils.h"
#include <stdbool.h>
#include <stdio.h>


int main(int argc, char **argv) {
  (void)argc; (void)argv;
  bool OK = isAuthorizedForAccessibility();
  log_debug("isAuthorizedForAccessibility:%d\n", OK);


  if(!isAuthorizedForScreenRecording()) DIE("not authorized to do screen recording");
  if(!isAuthorizedForAccessibility()) DIE("not authorized to use accessibility API");

  return(0);
}
