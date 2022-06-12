#define APPLICATION_NAME    "app-utils-test"
#include "app-utils.h"
#include "submodules/log.h/log.h"
#include "window-utils-test.h"
#include "window-utils.h"


int main(int argc, char **argv) {
  (void)argc; (void)argv;
  bool OK = isAuthorizedForAccessibility();
  log_debug("isAuthorizedForAccessibility:%d\n", OK);
  return(0);
}
