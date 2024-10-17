#include "../include/window.h"

xcb_atom_t get_atom(xcb_connection_t *connection, const char *atom_name) {
  xcb_intern_atom_cookie_t cookie =
      xcb_intern_atom(connection, 0, strlen(atom_name), atom_name);
  xcb_intern_atom_reply_t *reply =
      xcb_intern_atom_reply(connection, cookie, NULL);
  xcb_atom_t atom = reply->atom;
  free(reply);
  return atom;
}

void set_window_opacity(xcb_connection_t *connection, xcb_window_t window,
                        uint32_t opacity) {
  xcb_atom_t opacity_atom = get_atom(connection, "_NET_WM_WINDOW_OPACITY");
  xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, opacity_atom,
                      XCB_ATOM_CARDINAL, 32, 1, &opacity);
}

void set_transparency_for_window_and_children(xcb_connection_t *connection,
                                              xcb_window_t window,
                                              xcb_window_t exclude,
                                              float opacity) {
  if (window == exclude)
    return;

  set_window_opacity(connection, window, opacity);

  xcb_query_tree_cookie_t tree_cookie = xcb_query_tree(connection, window);
  xcb_query_tree_reply_t *tree_reply =
      xcb_query_tree_reply(connection, tree_cookie, NULL);
  if (!tree_reply) {
    return;
  }

  xcb_window_t *children = xcb_query_tree_children(tree_reply);
  int children_len = xcb_query_tree_children_length(tree_reply);

  for (int i = 0; i < children_len; i++) {
    set_transparency_for_window_and_children(connection, children[i], exclude,
                                             opacity);
  }

  free(tree_reply);
}

void set_transparency_for_all_windows(xcb_connection_t *connection,
                                      xcb_window_t exclude, uint32_t opacity) {
  xcb_screen_iterator_t iter;
  xcb_screen_t *screen;
  xcb_window_t root;

  iter = xcb_setup_roots_iterator(xcb_get_setup(connection));
  screen = iter.data;
  root = screen->root;

  set_transparency_for_window_and_children(connection, root, exclude, opacity);
}

xcb_window_t get_active_window(xcb_connection_t *connection) {
  xcb_atom_t active_window_atom = get_atom(connection, "_NET_ACTIVE_WINDOW");
  xcb_screen_iterator_t iter =
      xcb_setup_roots_iterator(xcb_get_setup(connection));
  xcb_screen_t *screen = iter.data;
  xcb_window_t root = screen->root;
  xcb_get_property_cookie_t cookie = xcb_get_property(
      connection, 0, root, active_window_atom, XCB_ATOM_WINDOW, 0, 1);

  xcb_get_property_reply_t *reply =
      xcb_get_property_reply(connection, cookie, NULL);
  if (reply == NULL) {
    printf("failed to get reply\n");
    return EXIT_FAILURE;
  }

  xcb_window_t active_window = *(xcb_window_t *)xcb_get_property_value(reply);

  free(reply);
  return active_window;
}