#include "../include/window.h"

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("failed to set transparency\n");
    return EXIT_FAILURE;
  }

  xcb_connection_t *connection = xcb_connect(NULL, NULL);
  if (xcb_connection_has_error(connection)) {
    fprintf(stderr, "failed to open a connection to the X-Server\n");
    return EXIT_FAILURE;
  }

  float opacity = (float)atof(argv[1]);
  uint32_t active_opacity = (unsigned long)(opacity * (float)OPAQUE);
  xcb_window_t last_active_window;

  while (1) {
    xcb_window_t active_window = get_active_window(connection);
    set_transparency_for_all_windows(connection, active_window, active_opacity);

    if (last_active_window != active_window) {
      last_active_window = active_window;
      set_window_opacity(connection, active_window, OPAQUE);
    }
  }

  xcb_flush(connection);
  xcb_disconnect(connection);
  return EXIT_SUCCESS;
}