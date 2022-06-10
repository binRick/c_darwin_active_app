#include "active-app-test.h"
#define WATCH_INTERVAL_MS    200
bool RUNNING = true;


void show_focus(){
  focused_t *fp = get_focused_process();

  printf("<%d> Focus: %s / %d\n", getpid(), fp->name, fp->pid);
}


int main(int argc, char **argv) {
  (void)argc; (void)argv;
  if ((argc >= 2) && (strcmp(argv[1], "--test") == 0)) {
    show_focus();
    printf("OK\n");
    return(0);
  }
  if ((argc >= 2) && (strcmp(argv[1], "--watch") == 0)) {
    while (RUNNING == true) {
      show_focus();
      usleep(WATCH_INTERVAL_MS * 1000);
    }
    return(0);
  }
  show_focus();
  return(0);
}
